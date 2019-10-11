import { Component, OnInit } from '@angular/core';
import { BluetoothCore } from '@manekinekko/angular-web-bluetooth';

@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.scss']
})
export class AppComponent implements OnInit {
  title = 'webapp';

  constructor(private ble: BluetoothCore) { }

  ngOnInit() {

  }

  cerca() {
    console.log('Cerco dispositivi bluetooth...')
  }
}
