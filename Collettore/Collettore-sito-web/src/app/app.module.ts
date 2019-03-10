import { BrowserModule } from '@angular/platform-browser'
import { NgModule } from '@angular/core'

import { AppRoutingModule } from './app-routing.module'
import { AppComponent } from './app.component'

import { AngularFireModule } from '@angular/fire'
import { environment } from '../environments/environment'
import { AngularFirestoreModule } from '@angular/fire/firestore'
import { AgmCoreModule } from '@agm/core'

import { MatToolbarModule } from '@angular/material/toolbar'
import { BrowserAnimationsModule } from '@angular/platform-browser/animations'
import { MatIconModule } from '@angular/material/icon'
import { MainMenuComponent } from './main-menu/main-menu.component'
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

import { FlexLayoutModule } from '@angular/flex-layout'
import { MappaComponent } from './mappa/mappa.component'
import { GraficiComponent } from './grafici/grafici.component'
import { TabelleComponent } from './tabelle/tabelle.component'
import { RecordComponent } from './tabelle/record/record.component'
import { BarchetteComponent } from './tabelle/barchette/barchette.component'
import { SensoriComponent } from './tabelle/sensori/sensori.component'
import { DialogDatiSensoreComponent } from './tabelle/record/dialog-dati-sensore/dialog-dati-sensore.component'
import { AngularFireDatabaseModule } from '@angular/fire/database'

@NgModule({
  declarations: [
    AppComponent,
    MainMenuComponent,
    MappaComponent,
    GraficiComponent,
    TabelleComponent,
    RecordComponent,
    BarchetteComponent,
    SensoriComponent,
    DialogDatiSensoreComponent,
  ],
  imports: [
    BrowserModule,
    AppRoutingModule,
    BrowserAnimationsModule,
    FlexLayoutModule,

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

    //angular google maps
    AgmCoreModule.forRoot({
      apiKey: 'AIzaSyD9J-60bBAsiwIlgNpWnnDKegftOZTPnh0'
    }),

    //firebase
    AngularFireModule.initializeApp(environment.firebase),
    AngularFirestoreModule,
    AngularFireDatabaseModule
  ],
  entryComponents: [
    DialogDatiSensoreComponent
  ],
  providers: [],
  bootstrap: [AppComponent],
})
export class AppModule { }
