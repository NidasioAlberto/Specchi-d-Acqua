import { DocumentReference } from '@angular/fire/firestore'
import { Sensore } from './sensore'
import { Observable } from 'rxjs'

export interface Barchetta {
    nome: string
    note: string
    versioneDesign: number
    alimentazioneFotovoltaica: number
    sensori: DocumentReference[]
    datiSensori?: Sensore[]
    ultimaPosizione?: Observable<{
        latitudine: number
        longitudine: number
        dateTime: string
    }>
    id?: string
}