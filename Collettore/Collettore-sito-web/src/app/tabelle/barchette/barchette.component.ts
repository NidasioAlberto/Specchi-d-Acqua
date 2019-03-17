import { Component, OnInit } from '@angular/core';
import { DatabaseService } from 'src/app/core/database.service';
import { Barchetta } from 'src/app/core/types/barchetta';
import { Observable } from 'rxjs';
import { Sensore } from 'src/app/core/types/sensore';
import { MatDialog } from '@angular/material';
import { DialogBarchettaComponent } from './dialog-barchetta/dialog-barchetta.component';

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
    //recupero la lista dei sensori
    barchetta.datiSensori = []
    barchetta.sensori.forEach(sensore => {
      sensore.get().then(value => {
        let tmp = value.data() as Sensore
        tmp.id = value.id
        barchetta.datiSensori.push(tmp)
      })
    })

    //e l'ultima posizione conosciuta
    if(barchetta.id != undefined) barchetta.ultimaPosizione = this.database.ottieniUltimaPosizioneBarchetta(barchetta.id)

    //TODO: come per i record questa funzione non dovrebbe recuperare i dati se sono già presenti, basta aggiungere un controllo
  }

  modificaBarchetta(barchetta: Barchetta) {
    this.dialog.open(DialogBarchettaComponent, { data: barchetta }).afterClosed().subscribe(console.log)
  }
}