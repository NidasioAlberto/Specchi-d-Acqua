import { BrowserModule } from '@angular/platform-browser'
import { NgModule } from '@angular/core'

import { AppRoutingModule } from './app-routing.module'
import { AppComponent } from './app.component'
import { BrowserAnimationsModule } from '@angular/platform-browser/animations'
import { MappaComponent } from './mappa/mappa.component'
import { GraficiComponent } from './grafici/grafici.component'
import { TabelleComponent } from './tabelle/tabelle.component'
import { MenuPrincipaleComponent } from './menu-principale/menu-principale.component'
import { ImpostazioniGraficiComponent } from './menu-principale/impostazioni-grafici/impostazioni-grafici.component'

import { MatToolbarModule } from '@angular/material/toolbar'
import { MatIconModule } from '@angular/material/icon'
import { MatCardModule } from '@angular/material/card'
import { MatButtonModule } from '@angular/material/button'
import { MatTabsModule } from '@angular/material/tabs'
import { MatTableModule } from '@angular/material/table'
import { MatGridListModule } from '@angular/material/grid-list'
import { MatChipsModule } from '@angular/material/chips'
import { MatDialogModule } from '@angular/material/dialog'
import { MatExpansionModule } from '@angular/material/expansion'
import { MatSlideToggleModule } from '@angular/material/slide-toggle'
import { MatListModule } from '@angular/material/list'
import { MatInputModule } from '@angular/material/input'
import { MatSelectModule } from '@angular/material/select'
import { MatBottomSheetModule } from '@angular/material/bottom-sheet'
import { MatButtonToggleModule } from '@angular/material/button-toggle'

import { ChartsModule } from 'ng2-charts'
import { HttpClientModule } from '@angular/common/http'
import { MqttModule, IMqttServiceOptions } from 'ngx-mqtt'
import { AngularFireModule } from '@angular/fire'
import { environment } from '../environments/environment'
import { AngularFireDatabaseModule } from '@angular/fire/database'
import { AngularFirestoreModule } from '@angular/fire/firestore'
import { BarchetteComponent } from './tabelle/barchette/barchette.component'
import { RecordComponent } from './tabelle/record/record.component'
import { SensoriComponent } from './tabelle/sensori/sensori.component'
import { AgmCoreModule } from '@agm/core';


export const MQTT_SERVICE_OPTIONS: IMqttServiceOptions = {
  hostname: '192.168.4.100',
  port: 9001,
  path: '/mqtt'
}

@NgModule({
  declarations: [
    AppComponent,
    MappaComponent,
    GraficiComponent,
    TabelleComponent,
    MenuPrincipaleComponent,
    ImpostazioniGraficiComponent,
    BarchetteComponent,
    RecordComponent,
    SensoriComponent,
  ],
  imports: [
    BrowserModule,
    AppRoutingModule,
    BrowserAnimationsModule,

    //componenti material design
    MatToolbarModule,
    MatIconModule,
    MatCardModule,
    MatButtonModule,
    MatTabsModule,
    MatTableModule,
    MatGridListModule,
    MatChipsModule,
    MatDialogModule,
    MatExpansionModule,
    MatSlideToggleModule,
    MatListModule,
    MatInputModule,
    MatSelectModule,
    MatBottomSheetModule,
    MatButtonToggleModule,

    ChartsModule,
    HttpClientModule,
    MqttModule.forRoot(MQTT_SERVICE_OPTIONS),
    AngularFireModule.initializeApp(environment.firebase),
    AngularFireDatabaseModule,
    AngularFirestoreModule,

    //angular google maps
    AgmCoreModule.forRoot({
      apiKey: 'AIzaSyD9J-60bBAsiwIlgNpWnnDKegftOZTPnh0'
    }),
  ],
  providers: [],
  bootstrap: [AppComponent],
  entryComponents: [
    ImpostazioniGraficiComponent
  ]
})
export class AppModule { }
