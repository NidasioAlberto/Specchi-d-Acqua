import { Record, DatiSensore} from './tipi'

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

export function controlloDatiSensore(obj: any): obj is DatiSensore {
    return obj.valore != undefined &&
           obj.idSensore != undefined
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