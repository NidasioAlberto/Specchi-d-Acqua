import { DocumentReference } from '@angular/fire/firestore';

export interface DatiSensore {
    idSensore: DocumentReference,
    valore: number
}