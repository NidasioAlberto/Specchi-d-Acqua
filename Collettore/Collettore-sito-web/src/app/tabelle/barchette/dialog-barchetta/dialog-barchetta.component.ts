import { Component, OnInit, Inject } from '@angular/core';
import { MatDialogRef, MAT_DIALOG_DATA } from '@angular/material';
import { Barchetta } from 'src/app/core/types/barchetta';
import { DatabaseService } from 'src/app/core/database.service';
import { FormControl } from '@angular/forms';
import { Observable } from 'rxjs';
import { Sensore } from 'src/app/core/types/sensore';

@Component({
  selector: 'app-dialog-barchetta',
  templateUrl: './dialog-barchetta.component.html',
  styleUrls: ['./dialog-barchetta.component.scss']
})
export class DialogBarchettaComponent implements OnInit {

  mode: 'new' | 'edit' = 'new'
  sensoriSelezionati = new FormControl();
  barchetta: Barchetta
  sensori: Observable<Sensore[]>

  constructor(public dialogRef: MatDialogRef<DialogBarchettaComponent>,
    @Inject(MAT_DIALOG_DATA) public data: Barchetta,
    public database: DatabaseService) { }

  ngOnInit() {
    if(this.data != undefined) {
      this.mode = 'edit'
      this.barchetta = this.data
    }

    this.sensori = this.database.ottieniSensori()
    this.sensori.subscribe(() => this.sensoriSelezionati.setValue(this.data.datiSensori.map(sensore => sensore.id)))
  }

  salva() {
    console.log(this.data)
    console.log(this.sensoriSelezionati)
  }
}
