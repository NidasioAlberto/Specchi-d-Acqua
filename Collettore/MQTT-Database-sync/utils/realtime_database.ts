import { database } from 'firebase-admin'
import { Record } from './tipi'
import { controlloRecord } from './controllo_dati';

export class RealtimeDatabase {
    database: database.Database

    constructor() {
        this.database = database()
    }

    salvaRecord(record: Record) {
        //controllo i dati
        if(!controlloRecord(record)) throw Error('Dati non validi')

        return new Promise<void>((resolve, reject) => {
            this.database.ref('Record').push(record, (err) => {
                if(err) reject(err)
                else {
                    console.log('Dati salvati sul realtime database')
                    resolve()
                }
            })
        })
    }
}