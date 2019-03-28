import mqtt = require('mqtt')
import { Database } from './utils/database'
import { RealtimeDatabase } from './utils/realtime_database'

const INDIRIZZO_SERVER = '192.168.4.100'

import { initializeApp, credential} from 'firebase-admin'

/*var serviceAccount = require('./service-account-key.json')
initializeApp({
    credential: credential.cert(serviceAccount),
    databaseURL: 'https://specchi-acqua.firebaseio.com/'
})*/

//const firebaseDb = new RealtimeDatabase()

const database = new Database({
    host     : INDIRIZZO_SERVER,
    user     : 'Collettore',
    password : 'Fornaroli',
    database : 'DatabaseBarchette'
})

const client = mqtt.connect('mqtt://' + INDIRIZZO_SERVER)

client.on('connect', function () {
    console.log('Collegamento a mosquitto effettuato')
    client.subscribe('records', function (err) {
        console.log('Subscription a records pronta')
    })
})

client.on('message', async (topic, message) => {
    console.log(topic, message.toString())

    //decodifico il messaggio
    let dati = JSON.parse(message.toString())

    //salvo i dati
    try {
        await database.salvaRecord(dati)

        //await firebaseDb.salvaRecord(dati)
    } catch(error) {
        console.error(error)
    }
})