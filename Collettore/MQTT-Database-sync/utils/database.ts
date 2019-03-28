import { Connection, createConnection, ConnectionConfig } from 'mysql'
import { Record } from './tipi'
import { controlloRecord } from './controllo_dati';

export class Database {
    connection: Connection

    constructor(configurazione?: ConnectionConfig) {
        if(configurazione != undefined) this.connetti(configurazione)
    }

    /**
     * Apre la connessione con il database grazie ai parametri forniti
     * @param configurazione parametri per la connessione
     */
    connetti(configurazione: ConnectionConfig) {
        this.connection = createConnection(configurazione)

        return new Promise<any>((resolve, reject) => {
            this.connection.connect(err => {
                if(err) return reject(err);
                resolve();
            })
        })
    }

    /**
     * Esegue la query con i parametri specificati
     * @param sql query da eseguire
     * @param args parametri della query
     */
    query(sql: string, args?: any) {
        return new Promise<any>((resolve, reject) => {
            this.connection.query(sql, args, (err, rows) => {
                if(err) return reject(err);
                resolve(rows);
            });
        });
    }

    /**
     * Conclude la connessione con il database
     */
    chiudi() {
        return new Promise((resolve, reject) => {
            this.connection.end(err => {
                if(err) return reject(err);
                resolve();
            });
        });
    }

    async salvaRecord(record: Record) {
        console.log(record)

        //controllo i dati
        if(!controlloRecord(record)) throw Error('Dati non validi')

        try {
            let query = 'INSERT INTO record SET ?'

            let record2 = Object.assign({}, record)
            delete record2.datiSensori
        
            let result = await this.query(query, record2)
    
            if(record.datiSensori == undefined) return
    
            let idRecord: number = result.insertId
            query = "INSERT INTO datiSensori SET ?"
    
            //TODO: rivedere l'errore in caso si aggiungano sensori che non esistono ancora, per adesso viene creata al barchetta, i sensori non vengono aggiunti ma il risultato è positivo
            let promesse = []
            
            record.datiSensori.forEach(datiSensore => {
                datiSensore.idRecord = idRecord
                promesse.push(this.query(query, datiSensore))
            })
    
            await Promise.all(promesse)
    
            console.log('Dati salvati sul database sql')
        } catch(error) {
            throw Error('Imposssibile salvare i dati: ' +  error)
        }

    }
}