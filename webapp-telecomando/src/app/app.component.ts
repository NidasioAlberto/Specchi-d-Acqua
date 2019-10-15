import { Component, OnInit } from '@angular/core'
import { map } from 'rxjs/operators'
import { SENSORS_SERVICE_UUID, WATER_TEMPERATURE_CHARACTERISTIC_UUID, GPS_LOCATION_SERVICE_UUID, CONTROL_SERVICE_UUID, AIR_TEMPERATURE_CHARACTERISTIC_UUID, AIR_HUMIDITY_CHARACTERISTIC_UUID, GPS_LATITUDE_CHARACTERISTIC_UUID, GPS_LONGITUDE_CHARACTERISTIC_UUID, GPS_SATELLITES_CHARACTERISTIC_UUID, GPS_ALTITUDE_CHARACTERISTIC_UUID, SPEED_CHARACTERISTIC_UUID, GPS_SPEED_CHARACTERISTIC_UUID, GPS_HDOP_CHARACTERISTIC_UUID, DIRECTION_CHARACTERISTIC_UUID} from './utils/uuids'
import { Observable, fromEvent } from 'rxjs';

@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.scss']
})
export class AppComponent implements OnInit {
  title = 'webapp';

  private ble: Bluetooth;

  server: BluetoothDevice
  gatt: BluetoothRemoteGATTServer
  serverConnected: boolean = false

  sensorsService: BluetoothRemoteGATTService
  airTemperature: Observable<string>
  airTemperatureCharacteristic: BluetoothRemoteGATTCharacteristic
  airHumidity: Observable<string>
  airHumidityCharacteristic: BluetoothRemoteGATTCharacteristic
  waterTemperature: Observable<string>
  waterTemperatureCharacteristic: BluetoothRemoteGATTCharacteristic

  gpsLocationService: BluetoothRemoteGATTService
  gpsLatitude: Observable<string>
  gpsLatitudeCharacteristic: BluetoothRemoteGATTCharacteristic
  gpsLongitude: Observable<string>
  gpsLongitudeCharacteristic: BluetoothRemoteGATTCharacteristic
  gpsSatellites: Observable<string>
  gpsSatellitesCharacteristic: BluetoothRemoteGATTCharacteristic
  gpsAltitude: Observable<string>
  gpsAltitudeCharacteristic: BluetoothRemoteGATTCharacteristic
  gpsSpeed: Observable<string>
  gpsSpeedCharacteristic: BluetoothRemoteGATTCharacteristic
  gpsHdop: Observable<string>
  gpsHdopCharacteristic: BluetoothRemoteGATTCharacteristic

  controlService: BluetoothRemoteGATTService
  directionCharacteristic: BluetoothRemoteGATTCharacteristic
  speedCharacteristic: BluetoothRemoteGATTCharacteristic

  writeInProgress: boolean = false

  constructor() {
    this.ble = navigator.bluetooth
  }

  ngOnInit() { }

  async connect() {
    try {
      // Search the devices
      this.server = await this.ble.requestDevice({
        filters: [
          {
            namePrefix: 'Barchetta',
          }
        ],
        optionalServices: [
          SENSORS_SERVICE_UUID,
          GPS_LOCATION_SERVICE_UUID,
          CONTROL_SERVICE_UUID
        ],
      })
  
      if(this.server !== null) {
        this.server.ongattserverdisconnected = (event: Event) => {
          this.serverConnected = false
          console.log('Lost connection to the server')
        }

        this.gatt = await this.server.gatt.connect()
        this.serverConnected = true
  
        // Retrie the sensors service and get the characteristics
        this.sensorsService = await this.gatt.getPrimaryService(SENSORS_SERVICE_UUID)

        this.airTemperatureCharacteristic = await this.sensorsService.getCharacteristic(AIR_TEMPERATURE_CHARACTERISTIC_UUID)
        this.airTemperature = await this.retrieveCharacteristicValueUpdates(this.airTemperatureCharacteristic)

        this.airHumidityCharacteristic = await this.sensorsService.getCharacteristic(AIR_HUMIDITY_CHARACTERISTIC_UUID)
        this.airHumidity = await this.retrieveCharacteristicValueUpdates(this.airHumidityCharacteristic)

        this.waterTemperatureCharacteristic = await this.sensorsService.getCharacteristic(WATER_TEMPERATURE_CHARACTERISTIC_UUID)
        this.waterTemperature = await this.retrieveCharacteristicValueUpdates(this.waterTemperatureCharacteristic)
  
        // Retrieve the gps location service and get the characteristics
        this.gpsLocationService = await this.gatt.getPrimaryService(GPS_LOCATION_SERVICE_UUID)

        this.gpsLatitudeCharacteristic = await this.gpsLocationService.getCharacteristic(GPS_LATITUDE_CHARACTERISTIC_UUID)
        this.gpsLatitude = await this.retrieveCharacteristicValueUpdates(this.gpsLatitudeCharacteristic)

        this.gpsLongitudeCharacteristic = await this.gpsLocationService.getCharacteristic(GPS_LONGITUDE_CHARACTERISTIC_UUID)
        this.gpsLongitude = await this.retrieveCharacteristicValueUpdates(this.gpsLongitudeCharacteristic)

        this.gpsSatellitesCharacteristic = await this.gpsLocationService.getCharacteristic(GPS_SATELLITES_CHARACTERISTIC_UUID)
        this.gpsSatellites = await this.retrieveCharacteristicValueUpdates(this.gpsSatellitesCharacteristic)

        this.gpsAltitudeCharacteristic = await this.gpsLocationService.getCharacteristic(GPS_ALTITUDE_CHARACTERISTIC_UUID)
        this.gpsAltitude = await this.retrieveCharacteristicValueUpdates(this.gpsAltitudeCharacteristic)

        this.gpsSpeedCharacteristic = await this.gpsLocationService.getCharacteristic(GPS_SPEED_CHARACTERISTIC_UUID)
        this.gpsSpeed = await this.retrieveCharacteristicValueUpdates(this.gpsSpeedCharacteristic)
        
        this.gpsHdopCharacteristic = await this.gpsLocationService.getCharacteristic(GPS_HDOP_CHARACTERISTIC_UUID)
        this.gpsHdop = await this.retrieveCharacteristicValueUpdates(this.gpsHdopCharacteristic)

        // Retrieve the control service and its characteristics
        this.controlService = await this.gatt.getPrimaryService(CONTROL_SERVICE_UUID)
        this.speedCharacteristic = await this.controlService.getCharacteristic(SPEED_CHARACTERISTIC_UUID)
        this.directionCharacteristic = await this.controlService.getCharacteristic(DIRECTION_CHARACTERISTIC_UUID)
      } else {
        console.log('The user didn\'t select a device')
      }
    } catch(err) {
      console.log('Something went wrong', err)
    }
  }

  disconnect() {
    this.gatt.disconnect()
  }

  toggleConnection() {
    if(this.serverConnected) {
      this.disconnect()
    } else {
      this.connect()
    }
  }

  /**
   * This function start the notification on a charactristic and return an observable of its values
   * @param characteristic The characteristic from where we'll get the values
   */
  async retrieveCharacteristicValueUpdates(characteristic: BluetoothRemoteGATTCharacteristic) {
    characteristic = await characteristic.startNotifications()

    return fromEvent(characteristic, 'characteristicvaluechanged').pipe(
      map(event => (new TextDecoder()).decode((event.srcElement as BluetoothRemoteGATTCharacteristic).value.buffer))
    )
  }

  async updateControls(controls: { direction: number, speed: number}) {
    console.log('Update controls, direction:', controls.direction, 'speed', controls.speed)

    if(!this.writeInProgress) {
      this.writeInProgress = true
      if(this.directionCharacteristic !== undefined) await this.directionCharacteristic.writeValue((new TextEncoder()).encode(controls.direction.toString()))
      if(this.speedCharacteristic != undefined) await this.speedCharacteristic.writeValue((new TextEncoder()).encode(Math.round(controls.speed).toString()))
      console.log('data sent')
      this.writeInProgress = false
    }
  }
}
