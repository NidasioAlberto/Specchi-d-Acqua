/**
 * Interfaccia per i dati di un record
 */
export interface Record {
    latitude: number
    longitudine: number
    heading: number
    dateTime: string
    altitudine: string
    hdop: number
    satelliti: number
    velocita: number
    idBarchetta: string
    id: number
    datiSensori?: DatiSensore[]
}

/**
 * Interfaccia per i dati di un sensore
 */
export interface DatiSensore {
    id: number
    valore: number
    idSensore: string
    idRecord: number
}

/**
 * Interfaccia per i dati di una barchetta
 */
export interface Barchetta {
    id: string
    nome: string,
    versioneDesign: number,
    alimentazioneFotovoltaica: boolean,
    note: string
    datiSensori?: Sensore[]
}

/**
 * Interfaccia per i dati di un sensore
 */
export interface Sensore {
    id: string,
    nome: string,
    note: string,
    unita: string
}