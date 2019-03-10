import { DocumentReference } from '@angular/fire/firestore'
import { Observable } from 'rxjs'
import { Sensore } from './sensore'

export interface DatiSensore {
    idSensore: DocumentReference,
    valore: number
    sensore: Observable<Sensore>
}