import { Component, OnInit, ViewChild, Input, SimpleChange, SimpleChanges } from '@angular/core'
import { DatabaseService } from '../core/database.service'
import { ChartDataSets, ChartOptions } from 'chart.js'
import { Label, Color, BaseChartDirective } from 'ng2-charts'
import { ActivatedRoute } from '@angular/router';
import { ImpostazioniGrafici } from '../core/tipi';
import { Subscription, combineLatest } from 'rxjs';
import { map } from 'rxjs/operators';

@Component({
  selector: 'app-grafici',
  templateUrl: './grafici.component.html',
  styleUrls: ['./grafici.component.scss']
})
export class GraficiComponent implements OnInit {

  datiGrafico: ChartDataSets[] = [{}]
  etichetteGrafico: Label[] = []
  opzioniGrafico: (ChartOptions & { annotation: any }) = {
    responsive: true,
    maintainAspectRatio: false,
    scales: {
      // We use this empty structure as a placeholder for dynamic theming.
      xAxes: [],
      yAxes: [
        {
          id: 'y-axis-0',
          position: 'left',
          ticks: {
            min: 0,
            max: 60
          }
        }
      ]
    },
    annotation: {
      annotations: [
        {
          type: 'line',
          mode: 'vertical',
          scaleID: 'x-axis-0',
          value: 'March',
          borderColor: 'orange',
          borderWidth: 2,
          label: {
            enabled: true,
            fontColor: 'orange',
            content: 'LineAnno'
          }
        },
      ],
    },
  }
  coloriDatiGrafico: Color[] = [
    { // barchetta 1 verde
      backgroundColor: 'rgba(31, 221, 43, 0.2)',
      borderColor: 'rgba(31, 221, 43,1)',
      pointBackgroundColor: 'rgba(31, 221, 43, 1)',
      pointBorderColor: '#fff',
      pointHoverBackgroundColor: '#fff',
      pointHoverBorderColor: 'rgba(31, 221, 43, 0.8)'
    },
    { //barca 2 blu
      backgroundColor: 'rgba(31, 135, 221, 0.2)',
      borderColor: 'rgba(31, 135, 221, 1)',
      pointBackgroundColor: 'rgba(31, 135, 221, 1)',
      pointBorderColor: '#fff',
      pointHoverBackgroundColor: '#fff',
      pointHoverBorderColor: 'rgba(31, 135, 221, 1)'
    },
    { //barchetta 3 arancio
      backgroundColor: 'rgba(255, 131, 0, 0.2)',
      borderColor: 'rgba(255, 131, 0, 1)',
      pointBackgroundColor: 'rgba(255, 131, 0, 1)',
      pointBorderColor: '#fff',
      pointHoverBackgroundColor: '#fff',
      pointHoverBorderColor: 'rgba(255, 131, 0, 1)'
    },
  ];

  currentSubscription: Subscription

  impostazioniCorrenti: ImpostazioniGrafici = {
    modalita: 'sensori',
    oggetto: '1'
  }

  @ViewChild(BaseChartDirective) grafico: BaseChartDirective;

  constructor(private database: DatabaseService, private route: ActivatedRoute) { }

  ngOnInit() {
    this.caricaDati(this.impostazioniCorrenti)

    this.route.queryParams.subscribe((impostazioni: ImpostazioniGrafici) => {
      if(impostazioni != undefined) {
        this.impostazioniCorrenti = impostazioni
        this.caricaDati(impostazioni)
      }
    })
  }

  caricaDati(impostazioni: ImpostazioniGrafici) {
    if(impostazioni.modalita == 'sensori') {
      if(this.currentSubscription != undefined) this.currentSubscription.unsubscribe()
      this.currentSubscription = this.database.ottieniDatiSensoreDivisiPerBarchette(impostazioni.oggetto, 60).subscribe(datiSensoreDivisi => {
        this.etichetteGrafico = []
        this.datiGrafico = []
        datiSensoreDivisi.forEach(datiSensore => {
          this.datiGrafico.push({
            label: datiSensore.barchetta.nome,
            fill: false,
            data: datiSensore.datiSensore.map((el, index) => {
              return {
                y: el.valore
              }
            })
          })
    
          if(this.etichetteGrafico.length < datiSensore.datiSensore.length)
            for(let i = 0; i < datiSensore.datiSensore.length; i++) this.etichetteGrafico.push(String(i))
            this.grafico.update()
        })
      })
    } else if(impostazioni.modalita == 'barchette') {
      if(this.currentSubscription != undefined) this.currentSubscription.unsubscribe()
      this.currentSubscription = combineLatest(
        this.database.ottieniRecord(60, impostazioni.oggetto),
        this.database.ottieniBarchetta(impostazioni.oggetto)).pipe(
        map(data => {
          return {
            record: data[0],
            barchetta: data[1]
          }
        })
      ).subscribe(data => {
        this.etichetteGrafico = []
        this.datiGrafico = []
        data.barchetta.datiSensori.forEach(sensore => {
          this.datiGrafico.push({
            label: sensore.nome,
            fill: false,
            data: data.record.map(recordSingolo => {
              return {
                y: recordSingolo.datiSensori.find(dato => {
                  return (dato.idSensore == sensore.id)
                })
              }
            })/*.filter(element => {
              return element.y != undefined
            })*/.map(element => {
              return (element.y == undefined ? NaN : element.y.valore)
            })
          })
    
          if(this.etichetteGrafico.length < data.record.length)
            for(let i = 0; i < data.record.length; i++) this.etichetteGrafico.push(String(i))
            this.grafico.update()
        })
      })
    }
  }
}