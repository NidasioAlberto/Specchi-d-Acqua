import {Component, Inject, OnInit} from '@angular/core'
import { MatDialogRef, MAT_DIALOG_DATA} from '@angular/material'
import { DatiSensore } from 'src/app/core/types/dati-sensore'
import { Observable } from 'rxjs'
import { Sensore } from 'src/app/core/types/sensore'
import { DatabaseService } from 'src/app/core/database.service'

@Component({
  selector: 'app-dialog-dati-sensore',
  templateUrl: './dialog-dati-sensore.component.html',
  styleUrls: ['./dialog-dati-sensore.component.scss']
})
export class DialogDatiSensoreComponent implements OnInit {

  sensore: Observable<Sensore>

  constructor(public dialogRef: MatDialogRef<DialogDatiSensoreComponent>,
    @Inject(MAT_DIALOG_DATA) public data: DatiSensore,
    public database: DatabaseService) { }
  
  ngOnInit() {
    this.sensore = this.database.ottieniSensore(this.data.idSensore)
  }
}
