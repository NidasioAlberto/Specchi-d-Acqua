import { DocumentReference } from '@angular/fire/firestore'
import { Timestamp } from 'rxjs/internal/operators/timestamp';
import { DatiSensore } from './dati-sensore';

export interface Record {
  record: {
    latitudine: number,
    longitudine: number,
    altitude: number,
    dateTime: any
    idBarchetta: DocumentReference,
    satelliti: number,
    velocita: number
  },
  datiSensori: DatiSensore[]
}
