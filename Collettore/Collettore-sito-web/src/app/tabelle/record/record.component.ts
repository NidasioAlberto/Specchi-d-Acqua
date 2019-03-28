import {Component, OnInit } from '@angular/core'
import { Observable } from 'rxjs'
import { animate, state, style, transition, trigger } from '@angular/animations'
import { MatDialog } from '@angular/material';
import { DatabaseService } from 'src/app/core/database.service';
import { DatiSensore, Record } from 'src/app/core/tipi';

@Component({
  selector: 'app-record',
  templateUrl: './record.component.html',
  styleUrls: ['./record.component.scss'],
  animations: [
    trigger('detailExpand', [
      state('collapsed', style({height: '0px', minHeight: '0', display: 'none'})),
      state('expanded', style({height: '*'})),
      transition('expanded <=> collapsed', animate('225ms cubic-bezier(0.4, 0.0, 0.2, 1)')),
    ]),
  ],
})
export class RecordComponent implements OnInit {

  record: Observable<Record[]>

  displayedColumns: string[] = ['id', 'latitudine', 'longitudine', 'altitudine', 'dateTime', 'satelliti', 'velocita']

  constructor(public database: DatabaseService, public dialog: MatDialog) { }

  ngOnInit() {
    this.record = this.database.ottieniRecord(16)
  }

  aperturaPannello(record: Record) {
    //recupero il nome della barchetta
    if(record.barchetta == undefined) record.barchetta = this.database.ottieniBarchetta(record.idBarchetta)

    //recupero i nomi dei sensori
    record.datiSensori = this.database.ottieniSensoriRecord(record.datiSensori)
    console.log(record)
    //TODO: quando si apre per la prima volta l'expansion panel l'id sparisce per un attimo e appare il nome, fin qui tutto bene
    //quando però viene chiuso e si riaspre la funzione viene richiamata, dovrebbe invece bloccarse senza ricaricare i dati, tanto sono Observable!
  }
}