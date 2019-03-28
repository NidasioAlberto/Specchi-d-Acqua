import { Record, DatiSensore, Barchetta, Sensore} from './tipi'

//SEZIONE RECORD

export function controlloRecord(obj: any): obj is Record {
    let controlloRecord = obj.latitudine != undefined &&
        obj.longitudine != undefined &&
        //obj.heading != undefined &&
        obj.altitudine != undefined &&
        obj.satelliti != undefined &&
        obj.hdop != undefined &&
        obj.velocita != undefined &&
        obj.dateTime != undefined &&
        obj.idBarchetta != undefined

    let controlloDatiSensori = true
    if(obj.datiSensori != undefined) controlloDatiSensori = controlloArrayDatiSensori(obj.datiSensori)

    return controlloRecord && controlloDatiSensori
}

export function controlloArrayDatiSensori(arr: any): arr is DatiSensore[] {
    let itIs: boolean = true

    if(arr instanceof Array) {
        arr.forEach(element => {
            if(!controlloDatiSensore(element)) {
                itIs = false
                return false
            }
        })
    } else {
        itIs = false
    }

    return(itIs)
}

export function controlloDatiSensore(obj: any): obj is DatiSensore {
    return obj.valore != undefined &&
           obj.idSensore != undefined
}

//SEZIONE BARCHETTE

export function controlloBarchetta(obj: any): obj is Barchetta {
    return obj.id != undefined &&
           obj.nome != undefined &&
           obj.versioneDesign != undefined &&
           obj.alimentazioneFotovoltaica != undefined &&
           obj.note != undefined
}

export function controlloArrayIdSensori(arr: any): arr is string[] {
    let itIs: boolean = true

    if(arr instanceof Array) {
        arr.forEach(element => {
            if(typeof element != 'string') {
                itIs = false
                return false
            }
        })
    } else {
        itIs = false
    }

    return(itIs)
}

//SEZIONE SENSORI
export function controlloSensore(obj: any): obj is Sensore {
    return obj.id != undefined &&
           obj.nome != undefined &&
           obj.note != undefined &&
           obj.unita != undefined
}