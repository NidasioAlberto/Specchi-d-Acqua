import { DocumentReference } from '@angular/fire/firestore'
import { Observable } from 'rxjs'

export interface Record {
  id?: string
  latitudine: number
  longitudine: number
  heading: number
  dateTime: string | Date
  altitudine: number
  hdop: number
  satelliti: number
  velocita: number
  idBarchetta: string
  barchetta?: Observable<Barchetta>
  datiSensori: DatiSensore[]
}

export interface DatiSensore {
  idSensore: string
  valore: number
  nomeSensore?: string
  noteSensore?: string
  sensore?: Observable<Sensore>
}

export interface Sensore {
  id: string
  nome: string
  note: string
  unita: string
}

export interface Barchetta {
  id: string
  nome: string
  versioneDesign: number
  alimentazioneFotovoltaica: number | boolean
  note: string
  sensori: DocumentReference[]
  datiSensori: Sensore[]
  ultimaPosizione?: Observable<{
    latitudine: number,
    longitudine: number,
    dateTime: string | Date
  }>,
  colore: string
}

export interface ImpostazioniGrafici {
  modalita: string
  oggetto: string
}