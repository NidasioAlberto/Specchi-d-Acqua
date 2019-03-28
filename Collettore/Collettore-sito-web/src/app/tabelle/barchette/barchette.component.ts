import { Component, OnInit } from '@angular/core'
import { DatabaseService } from 'src/app/core/database.service'
import { Observable } from 'rxjs'
import { MatDialog } from '@angular/material'
import { Barchetta, Sensore } from 'src/app/core/tipi';

@Component({
  selector: 'app-barchette',
  templateUrl: './barchette.component.html',
  styleUrls: ['./barchette.component.scss']
})
export class BarchetteComponent implements OnInit {

  barchette: Observable<Barchetta[]>

  constructor(public database: DatabaseService, public dialog: MatDialog) { }

  ngOnInit() {
    this.barchette = this.database.ottieniBarchette()
  }

  aperturaPannello(barchetta: Barchetta) {
    //recupero l'ultima posizione conosciuta
    if(barchetta.id != undefined) barchetta.ultimaPosizione = this.database.ottieniUltimaPosizioneBarchetta(barchetta.id)

    //TODO: come per i record questa funzione non dovrebbe recuperare i dati se sono già presenti, basta aggiungere un controllo
  }

  modificaBarchetta(barchetta: Barchetta) {
    //this.dialog.open(DialogBarchettaComponent, { data: barchetta }).afterClosed().subscribe(console.log)
  }
}