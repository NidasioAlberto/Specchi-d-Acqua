import { BrowserModule } from '@angular/platform-browser';
import { NgModule } from '@angular/core';

import { AppRoutingModule } from './app-routing.module';
import { AppComponent } from './app.component';
import { ServiceWorkerModule } from '@angular/service-worker';
import { environment } from '../environments/environment';
import { BrowserAnimationsModule } from '@angular/platform-browser/animations';
import { JoystickComponent } from './joystick/joystick.component';
import { AngularResizedEventModule } from 'angular-resize-event';

import {
  MatButtonModule
} from '@angular/material'

import { 
  WebBluetoothModule
} from '@manekinekko/angular-web-bluetooth';

@NgModule({
  declarations: [
    AppComponent,
    JoystickComponent
  ],
  imports: [
    BrowserModule,
    AppRoutingModule,
    ServiceWorkerModule.register('ngsw-worker.js', { enabled: environment.production }),
    BrowserAnimationsModule,
    AngularResizedEventModule,

    WebBluetoothModule.forRoot({
      enableTracing: true // or false, this will enable logs in the browser's console
    }),

    // Material components
    MatButtonModule
  ],
  providers: [],
  bootstrap: [AppComponent]
})
export class AppModule { }
