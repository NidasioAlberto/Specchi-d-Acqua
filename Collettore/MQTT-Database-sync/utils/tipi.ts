export interface Record {
    latitude: number
    longitudine: number
    heading: number
    satelliti: number
    hdop: number
    velocita: number
    dateTime: string
    idBarchetta: string
    datiSensori?: DatiSensore[]
}

export interface DatiSensore {
    valore: number
    idSensore: number
    idRecord?: number
}