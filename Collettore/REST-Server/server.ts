import express = require('express')
import cors = require('cors')
import { Database } from './utils/database'
import { controlloRecord, controlloSensore, controlloBarchetta, controlloArrayIdSensori } from './utils/controllo_dati';
import { Sensore, Barchetta } from './utils/tipi';

//preparo il database
const database = new Database({
    host     : '192.168.4.100',
    user     : 'Collettore',
    password : 'Fornaroli',
    database : 'DatabaseBarchette',
    dateStrings: true
})

//preparo express
const app = express()
app.use(express.json())
app.use(cors({origin: '*'}))

//rendo disponibile la pagina web nella root dell'url
app.use('/', express.static('website'))

//SEZIONE RECORD

//lista record con i dati dei sensori
app.get('/api/record', async (req, res) => {
    console.log(req.method, req.url, req.query, req.body)

    try {
        return res.send(await database.ottieniRecord(req.query.limite, req.query.idBarchetta))
    } catch(errore) {
        return res.status(500).send(errore.message)
    }
})

//creazione di un record
app.post('/api/record', async (req, res) => {
    console.log(req.method, req.url, req.query, req.body)

    let dati = req.body

    //controlli i dati
    if(controlloRecord(dati)) try {
        //salvo i dati
        await database.salvaRecord(dati)

        return res.sendStatus(200)
    } catch(errore) {
        return res.status(500).send(errore.message)
    }
    else return res.status(400).send('Dati inseriti errati')
})

//SEZIONE BARCHETTE

//lista barchette
app.get('/api/barchette', async (req, res) => {
    console.log(req.method, req.url, req.body)

    try {
        return res.send(await database.ottieniBarchette())
    } catch(errore) {
        return res.status(500).send(errore.message)
    }
})

//creazione di una barchetta con sensori
app.post('/api/barchette', async (req, res) => {
    console.log(req.method, req.url, req.query, req.body)

    let dati: {
        barchetta: Barchetta,
        sensori: string[]
    } = req.body

    //controlli i dati
    if(controlloBarchetta(dati.barchetta) && controlloArrayIdSensori(dati.sensori)) try {
        //creo la barchetta
        await database.salvaBarchetta(dati.barchetta, dati.sensori)

        return res.sendStatus(200)
    } catch(errore) {
        return res.status(500).send(errore.message)
    }
    else return res.status(400).send('Dati inseriti errati')
})

//ottieni singola barchetta
app.get('/api/barchette/:idBarchetta', async (req, res) => {
    console.log(req.method, req.url, req.body)

    if(req.params.idBarchetta != undefined) {
        try {
            return res.send(await database.ottieniBarchetta(req.params.idBarchetta))
        } catch(errore) {
            return res.status(500).send(errore.message)
        }
    }
})

//SEZIONE SENSORI

//lista sensori
app.get('/api/sensori', async (req, res) => {
    console.log(req.method, req.url, req.body)

    try {
        return res.send(await database.ottieniSensori())
    } catch(errore) {
        return res.status(500).send(errore.message)
    }
})

//singolo sensore
app.get('/api/sensori/:idSensore', async (req, res) => {
    console.log(req.method, req.url, req.body)

    try {
        return res.send(await database.ottieniSensore(req.params.idSensore))
    } catch(errore) {
        return res.status(500).send(errore.message)
    }
})

//creazione di un sensore
app.post('/api/sensori', async (req, res) => {
    console.log(req.method, req.url, req.query, req.body)

    let dati: Sensore = req.body

    //controlli i dati
    if(controlloSensore(dati)) try {
        //creo la barchetta
        await database.salvaSensore(dati)

        return res.sendStatus(200)
    } catch(errore) {
        return res.status(500).send(errore.message)
    }
    else return res.status(400).send('Dati inseriti errati')
})

//QUERY COMPOSTE

//lista dati sensore di una barchetta
app.get('/api/barchette/:idBarchetta/sensori/:idSensore/record', async (req, res) => {
    console.log(req.method, req.url, req.query, req.body)

    //controllo i parametri
    if(req.params.idBarchetta != undefined && req.params.idSensore != undefined) {
        try {
            return res.send(await database.ottieniDatiSensoreBarchetta(req.params.idBarchetta, req.params.idSensore, req.query.limite))
        } catch(e) {
            return res.status(500).send(e)
        }
    }
})

//apro il server sulla porta 80
app.listen(80, () => console.log('Server in ascolto sulla porta 80'))