import { Injectable } from '@angular/core'
import { HttpClient } from '@angular/common/http'
import { Observable, combineLatest } from 'rxjs'
import { map, concatAll, mergeAll } from 'rxjs/operators'
import { Record, Barchetta, Sensore, DatiSensore } from './tipi'
import { MqttService, IMqttMessage } from 'ngx-mqtt';
import { AngularFireDatabase } from '@angular/fire/database'
import { AngularFirestore } from '@angular/fire/firestore'
import { environment } from '../../environments/environment'

@Injectable({
  providedIn: 'root'
})
export class DatabaseService {

  modalita: 'online' | 'locale' = 'locale'//(environment.production ? 'online' : 'locale')

  constructor(private http: HttpClient, private mqttService: MqttService, private fDatabase: AngularFireDatabase, private firestore: AngularFirestore) {
    /*mqttService.onConnect.subscribe(data => console.log('onConnect', data))
    mqttService.onClose.subscribe(data => console.log('onClose', data))
    mqttService.onEnd.subscribe(data => console.log('onEnd', data))
    mqttService.onError.subscribe(data => console.log('onError', data))
    //mqttService.onMessage.subscribe(data => console.log('onMessage', data))
    mqttService.onOffline.subscribe(data => console.log('onOffline', data))
    mqttService.onReconnect.subscribe(data => console.log('onReconnect', data))
    mqttService.onSuback.subscribe(data => console.log('onSuback', data))*/

    //controllo se è possibile recuperare i dati da locale
    /*http.get('http://localhost/api').subscribe(data => {
      this.modalita = 'locale'
      console.log('Modalità: ', this.modalita)
    }, err => {
      this.modalita = 'online'
      console.log('Modalità: ', this.modalita)
    })*/
  }

  ottieniRecord(limite?: number, idBarchetta?: string, rimuoviNulli?: boolean): Observable<Record[]> {
    if(this.modalita == 'locale') return this.ottieniRecordDaLocale(limite, idBarchetta, rimuoviNulli)
    else return this.ottieniRecordDaRemoto(limite, idBarchetta, rimuoviNulli)
  }

  ottieniBarchette(): Observable<Barchetta[]> {
    if(this.modalita == 'locale') return this.ottieniBarchetteDaLocale()
    else return this.ottieniBarchetteDaRemoto()
  }

  ottieniRecordDivisiPerBarchette(limite?: number, rimuoviNulli?: boolean) {
    console.log('ottieniRecordDivisiPerBarchette')
    return this.ottieniBarchette().pipe(
      map(barchette => {
        return combineLatest(barchette.map(barchetta => this.ottieniRecord(limite, barchetta.id, rimuoviNulli).pipe(
          map(record => {
            return { record, barchetta }
          })
        )))
      }),
      concatAll()
    )
  }

  ottieniDatiSensoreDivisiPerBarchette(idSensore: string, limite?: number) {
    console.log('ottieniDatiSensoreDivisiPerBarchette')
    return this.ottieniBarchette().pipe(
      map(barchette => {
        return combineLatest(barchette.map(barchetta => /*(this.modalita == 'locale' ? 
          this.ottieniDatiSensoreDivisiPerBarchetteDaLocale(barchetta.id, idSensore, limite) :*/
          this.ottieniDatiSensoreDivisiPerBarchetteDaRemoto(barchetta.id, idSensore, limite)/*)*/.pipe(
          map(datiSensore => {
            return { barchetta, datiSensore}
          })
        )))
      }),
      concatAll()
    )
  }

  ottieniSensori(): Observable<Sensore[]> {
    if(this.modalita == 'locale') return this.ottieniSensoriDaLocale()
    else return this.ottieniSensoriDaRemoto()
  }

  ottieniSensore(idSensore: string): Observable<Sensore> {
    if(this.modalita == 'locale') return this.ottieniSensoreDaLocale(idSensore)
    else return this.ottieniSensoreDaRemoto(idSensore)
  }

  ottieniBarchetta(idBarchetta: string): Observable<Barchetta> {
    if(this.modalita == 'locale') return this.ottieniBarchettaDaLocale(idBarchetta)
    else return this.ottieniBarchettaDaRemoto(idBarchetta)
  }

  ottieniSensoriRecord(datiSensori: DatiSensore[]) {
    console.log('ottieniSensoriRecord')
    datiSensori.forEach(datiSensore => {
      datiSensore.sensore = this.ottieniSensore(datiSensore.idSensore)
    })

    return datiSensori
  }

  ottieniUltimaPosizioneBarchetta(idBarchetta: string) {
    console.log('ottieniUltimaPosizioneBarchetta')
    return this.fDatabase.list<Record>('Record', ref => ref.orderByChild('idBarchetta').equalTo(idBarchetta).limitToLast(1)).valueChanges().pipe(
      map(valori => {
        let valore = valori.pop()
        if(valore != undefined && valore.latitudine != 0 && valore.longitudine != 0) {
          let dato = {
            latitudine: valore.latitudine,
            longitudine: valore.longitudine,
            dateTime: valore.dateTime
          }
          return dato
        } else return undefined
      })
    )
  }

  //DATABASE LOCALE
  private ottieniRecordDaLocale(limite?: number, idBarchetta?: string, rimuoviNulli?: boolean): Observable<Record[]> {
    let params = {}

    if(limite != undefined) params['limite'] = limite
    if(idBarchetta != undefined) params['idBarchetta'] = idBarchetta
    if(rimuoviNulli != undefined) params['rimuoviNulli'] = rimuoviNulli

    //return this.http.get<Record[]>('http://localhost/api/record', { params })

    return new Observable(observer => {
      this.http.get<Record[]>('http://localhost/api/record', { params }).subscribe(httpData => {
        observer.next(httpData)

        this.mqttService.observe('records').subscribe((message: IMqttMessage) => {
          try {
            let mqttData: Record = JSON.parse(message.payload.toString())

            if(rimuoviNulli == undefined || (mqttData.latitudine != 0 || mqttData.longitudine != 0 )) {
              if(mqttData.idBarchetta == idBarchetta || idBarchetta == undefined) {
                if(httpData.length >= limite) httpData.shift()
                httpData.push(mqttData)
              }
  
              observer.next(httpData)
            }
          } catch(e) {
            console.error(e)
          }
        })
      }, err => {
        observer.error(err)
      })
    })
  }

  private ottieniBarchetteDaLocale(): Observable<Barchetta[]> {
    return this.http.get<Barchetta[]>('http://localhost/api/barchette')
  }

  private ottieniSensoriDaLocale(): Observable<Sensore[]> {
    return this.http.get<Sensore[]>('http://localhost/api/sensori')
  }

  private ottieniDatiSensoreDivisiPerBarchetteDaLocale(idBarchetta: string, idSensore: string, limite?: number): Observable<{ dateTime: string, valore: number }[]> {
    let params = {}

    if(limite != undefined) params['limite'] = limite
    //return this.http.get<{ dateTime: Date, valore: number }[]>('http://localhost/api/barchette/' + idBarchetta + '/sensori/' + idSensore + '/record?limite=2', { params })

    return new Observable(observer => {
      this.http.get<{ dateTime: string, valore: number }[]>('http://localhost/api/barchette/' + idBarchetta + '/sensori/' + idSensore + '/record?limite=2', { params }).subscribe(httpData => {
        observer.next(httpData)

        this.mqttService.observe('records').subscribe((message: IMqttMessage) => {
          try {
            let mqttData = JSON.parse(message.payload.toString())

            let datiSensore

            mqttData.datiSensori.forEach(element => {
              if(element.idSensore == idSensore) datiSensore = element
            });

            if(mqttData.idBarchetta == idBarchetta && datiSensore != undefined) {
              if(httpData.length >= limite) httpData.shift()
              httpData.push(datiSensore)
              observer.next(httpData)
            }
          } catch(e) {
            console.error(e)
          }
        })
      }, err => {
        observer.error(err)
      })
    })
  }

  private ottieniSensoreDaLocale(idSensore: string): Observable<Sensore> {
    return this.http.get<Sensore>('http://localhost/api/sensori/' + idSensore)
  }

  private ottieniBarchettaDaLocale(idBarchetta: string): Observable<Barchetta> {
    return this.http.get<Barchetta>('http://localhost/api/barchette/' + idBarchetta)
  }

  //DATABASE REMOTO
  private ottieniRecordDaRemoto(limite?: number, idBarchetta?: string, rimuoviNulli?: boolean) {
    console.log('ottieniRecordDaRemoto')

    return this.fDatabase.list<Record>('Record', ref => {
      let query
      if(idBarchetta == undefined) query = ref.orderByChild('dateTime')
      else query = ref.orderByChild('idBarchetta').equalTo(idBarchetta)
      if(limite > 0) query = query.limitToLast(limite)
      return query
    }).valueChanges().pipe(
      map(record => {
        record.forEach(element => {
          if(typeof element.latitudine == 'string') element.latitudine = parseFloat(element.latitudine as string)
          if(typeof element.longitudine == 'string') element.longitudine = parseFloat(element.longitudine as string)
          if(typeof element.altitudine == 'string') element.altitudine = parseFloat(element.altitudine as string)

          element.idBarchetta = this.firestore.collection('Barchette').doc(String(element.idBarchetta)).ref.id

          if(element.datiSensori != undefined) element.datiSensori.forEach(element2 => {
            element2.idSensore = this.firestore.collection('Sensori').doc(String(element2.idSensore)).ref.id
          })
        })
        return record//.reverse()
      })
    )
  }

  private ottieniBarchetteDaRemoto() {
    console.log('ottieniBarchetteDaRemoto')
    return this.firestore.collection<Barchetta>('Barchette').snapshotChanges().pipe(
      map(actions => {
        return actions.map(action => {
          let barchetta = action.payload.doc.data()
          barchetta.id = action.payload.doc.ref.id
          return barchetta
        })
      })
    )
  }

  private ottieniDatiSensoreDivisiPerBarchetteDaRemoto(idBarchetta: string, idSensore: string, limite?: number): Observable<{ dateTime: string, valore: number }[]> {
    console.log('ottieniDatiSensoreDivisiPerBarchetteDaRemoto')
    return this.fDatabase.list<Record>('Record', ref => {
      let query
      if(idBarchetta == undefined) query = ref.orderByChild('dateTime')
      else query = ref.orderByChild('idBarchetta').equalTo(idBarchetta)
      if(limite > 0) query = query.limitToLast(limite)
      return query
    }).valueChanges().pipe(
      map(record => {
        return record.map(recordSingolo => {
          let record2: { dateTime: string, valore: number } = {
            dateTime: recordSingolo.dateTime as string,
            valore: undefined
          }

          recordSingolo.datiSensori.forEach(datiSensore => {
            if(datiSensore.idSensore == idSensore) {
              record2.valore = datiSensore.valore
            }
          })

          record2.dateTime = recordSingolo.dateTime as string

          return record2
        })
      }),
      map(record => {
        return record.filter(element => {
          return element.valore != undefined
        })
      })
    )
  }

  private ottieniSensoriDaRemoto() {
    console.log('ottieniSensoriDaRemoto')
    return this.firestore.collection<Sensore>('Sensori').snapshotChanges().pipe(
      map(actions => actions.map(action => {
        let sensore = action.payload.doc.data()
        sensore.id = action.payload.doc.ref.id
        return sensore
      }))
    )
  }

  private ottieniSensoreDaRemoto(idSensore: string): Observable<Sensore> {
    console.log('ottieniSensoreDaRemoto')
    return this.firestore.collection('Sensori').doc<Sensore>(idSensore).valueChanges()
  }

  private ottieniBarchettaDaRemoto(idBarchetta: string): Observable<Barchetta> {
    console.log('ottieniBarchettaDaRemoto')
    return this.firestore.collection('Barchette').doc<Barchetta>(idBarchetta).snapshotChanges().pipe(
      map(barchetta => {
        let barchetta2 = barchetta.payload.data()

        /*let promesse = barchetta.payload.data().sensori.map(sensore => {
          console.log('sensore', sensore)
          return this.ottieniSensore(sensore.id)
        })

        return combineAll(promesse)*/

        return combineLatest(barchetta2.sensori.map(sensore => {
          return this.ottieniSensore(sensore.id)
        })).pipe(
          map(sensori => {
            barchetta2.datiSensori = sensori
            return barchetta2
          })
        )
      }),
      mergeAll(),
    )
  }
}
