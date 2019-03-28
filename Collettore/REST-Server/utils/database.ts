import { Connection, createConnection, ConnectionConfig } from 'mysql'
import { Record, Barchetta, Sensore } from './tipi'
import { controlloRecord } from './controllo_dati';

export class Database {
    connection: Connection

    constructor(configurazione?: ConnectionConfig) {
        try {
            if(configurazione != undefined) this.connetti(configurazione)
        } catch(e) {
            this.connection = undefined
        }
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

    //SEZIONE RECORD

    /**
     * Permette di ottenere tutti i record con la possibilità di limitarne il numero
     * @param limite il numero di record massimi da recuperare
     * @param idBarchetta 
     */
    async ottieniRecord(limite?: number, idBarchetta?: string): Promise<Record[]> {
        let query = 'SELECT record.latitudine, record.longitudine, record.heading, record.dateTime, record.altitudine, record.hdop, record.satelliti, record.velocita, barchette.id idBarchetta, barchette.nome, ' +
                'group_concat(concat(\'{"valore":\', datiSensori.valore, \',"idSensore":"\', datiSensori.idSensore, \'","nomeSensore":"\', sensori.nome, \'","noteSensore":"\', sensori.note, \'"}\')) datiSensori ' +
                'FROM record ' +
                'JOIN datiSensori ' +
                'ON record.id = datiSensori.idRecord ' +
                'JOIN sensori ' +
                'ON datiSensori.idSensore = sensori.id ' +
                'JOIN barchette ' +
                'ON record.idBarchetta = barchette.id '

        if(idBarchetta != undefined) query += ' WHERE record.idBarchetta = "' + idBarchetta + '" '
        query += 'GROUP BY record.id, record.latitudine, record.longitudine, record.heading, record.dateTime, barchette.id, barchette.nome ORDER BY record.dateTime DESC'
        if(limite != undefined) query += ' LIMIT ' + limite
        else query += ' LIMIT 20' //20 record al massimo se il limite non è specificato
        query += ';'
        
        try {
            let result: Record[] = await this.query(query)
            result.forEach(record => record.datiSensori = JSON.parse('[' + record.datiSensori + ']'))
    
            console.log('Dati letti dal database sql')

            return result
        } catch(e) {
            console.error(e)
            throw Error(e)
        }
    }

    /**
     * Permette di salvare un nuovo record sul database
     * @param record record da salvare
     */
    async salvaRecord(record: Record) {
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

    //SEZIONE BARCHETTE

    /**
     * Permette di ottenere un array con tutte le barchette
     */
    async ottieniBarchette(): Promise<Barchetta[]> {
        //anche questa query permette non solo di ottenere gli attributi di una barchetta ma anche tutti i suoi sensori
        let query = 'SELECT barchette.id, barchette.nome, barchette.versioneDesign, barchette.alimentazioneFotovoltaica, barchette.note, barchette.colore, ' +
                'group_concat(concat(\'{"id":\', sensori.id, \',"nome":"\', sensori.nome, \'","note":"\', sensori.note, \'"}\')) datiSensori ' +
                'FROM barchette ' +
                'LEFT JOIN barchettaSensore ON barchette.id = barchettaSensore.idBarchetta ' +
                'LEFT JOIN sensori on barchettaSensore.idSensore = sensori.id ' +
                'GROUP BY barchette.id, barchette.nome, barchette.versioneDesign, barchette.alimentazioneFotovoltaica;'
        
        try {
            let result: Barchetta[] = await this.query(query)
            result.forEach(barchetta => {
                if(barchetta.datiSensori != undefined) barchetta.datiSensori = JSON.parse('[' + barchetta.datiSensori + ']')
                else delete barchetta.datiSensori
            })
    
            console.log('Dati letti dal database sql')

            return result
        } catch(e) {
            console.error(e)
            throw Error(e)
        }
    }

    async ottieniBarchetta(idBarchetta): Promise<Barchetta> {
        let query = 'SELECT barchette.id, barchette.nome, barchette.versioneDesign, barchette.alimentazioneFotovoltaica, ' +
                'group_concat( concat(\'{"id":\', sensori.id, \',"nome":"\', sensori.nome, \'","note":"\', sensori.note, \'"}\')) datiSensori ' +
                'FROM barchette ' +
                'LEFT JOIN barchettaSensore ' +
                'ON barchette.id = barchettaSensore.idBarchetta ' +
                'LEFT JOIN sensori ' +
                'on barchettaSensore.idSensore = sensori.id ' +
                'WHERE barchette.id = \'' + idBarchetta + '\'' +
                'GROUP BY barchette.id, barchette.nome, barchette.versioneDesign, barchette.alimentazioneFotovoltaica;'
        
        try {
            
            let result: Barchetta[] = await this.query(query)
            result.forEach(barchetta => {
                if(barchetta.datiSensori != undefined) barchetta.datiSensori = JSON.parse('[' + barchetta.datiSensori + ']')
                else delete barchetta.datiSensori
            })
    
            console.log('Dati letti dal database sql')
            
            if(result.length > 0) return result[0]
            else return null
        } catch(e) {
            console.error(e)
            throw Error(e)
        }
    }

    /**
     * Permette di creare una nuova barchetta e di associargli dei sensori
     * @param barchetta barchetta da creare
     * @param sensori sensori della barchetta
     */
    async salvaBarchetta(barchetta: Barchetta, sensori: string[]) {
        try {
            let query = 'INSERT INTO barchette SET ?'

            await this.query(query, barchetta)

            //TODO: rivedere l'errore in caso si aggiungano sensori che non esistono ancora, per adesso viene creata al barchetta, i sensori non vengono aggiunti ma il risultato è positivo
            let promesse = []
            query = "INSERT INTO barchettaSensore SET ?"
                
            sensori.forEach(idSensore => {
                promesse.push(this.query(query, { idBarchetta: barchetta.id, idSensore }))
            })

            await Promise.all(promesse)
    
            console.log('Dati salvati sul database sql')
        } catch(error) {
            throw Error('Imposssibile salvare i dati della barchetta')
        }
    }

    //SEZIONE SENSORI

    /**
     * Permette di recuperare tutti i sensori disponibili
     */
    async ottieniSensori(): Promise<Sensore[]> {
        let query = 'SELECT * FROM sensori'

        try {
            let result: Sensore[] = await this.query(query)
    
            console.log('Dati letti dal database sql')

            return result
        } catch(e) {
            console.error(e)
            throw Error(e)
        }
    }

    /**
     * Permette di recuperare un sensore dato il suo id
     */
    async ottieniSensore(idSensore: string): Promise<Sensore> {
        let query = 'SELECT * FROM sensori WHERE id = \'' + idSensore + '\';'
        console.log(query)

        try {
            let result: Sensore = (await this.query(query))[0]
    
            console.log('Dati letti dal database sql')

            return result
        } catch(e) {
            console.error(e)
            throw Error(e)
        }
    }

    /**
     * Permette di creare un nuovo sensore
     * @param sensore sensore da creare
     */
    async salvaSensore(sensore: Sensore) {
        try {
            let query = 'INSERT INTO sensori SET ?'

            await this.query(query, sensore)
    
            console.log('Dati salvati nel database sql')
        } catch(error) {
            throw Error('Imposssibile salvare i dati della barchetta')
        }
    }

    //FUNZIONI MISTE

    async ottieniDatiSensoreBarchetta(idBarchetta: string, idSensore: string, limite?: number) {
        let query = 'select record.dateTime, datiSensori.valore ' +
            'from record ' +
            'join datiSensori ' +
            'on record.id = datiSensori.idRecord ' +
            'where record.idBarchetta = \'' + idBarchetta +'\' ' +
            'and datiSensori.idSensore = \'' + idSensore + '\' ORDER BY record.dateTime DESC'

        if(limite != undefined) query += ' LIMIT ' + limite
        query += ';'
        
        try {
            let result: {
                dateTime: string,
                valore: number
            }[] = await this.query(query)
    
            console.log('Dati letti dal database sql')
            
            return result
        } catch(e) {
            console.error(e)
            throw Error(e)
        }
    }
}