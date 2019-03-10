import { DocumentReference } from '@angular/fire/firestore'
import { DatiSensore } from './dati-sensore'
import { Observable } from 'rxjs'
import { Barchetta } from './barchetta'

export interface Record {
  latitudine: number,
  longitudine: number,
  altitudine: number,
  dateTime: any
  idBarchetta: DocumentReference,
  barchetta?: Observable<Barchetta>
  satelliti: number,
  velocita: number
  datiSensori: DatiSensore[]
}
