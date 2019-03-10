import { Component, OnInit, ViewChild } from '@angular/core'
import { ChartDataSets, ChartOptions } from 'chart.js'
import { Label, Color, BaseChartDirective } from 'ng2-charts'
import { DatabaseService } from '../core/database.service';

@Component({
  selector: 'app-grafici',
  templateUrl: './grafici.component.html',
  styleUrls: ['./grafici.component.scss']
})
export class GraficiComponent implements OnInit {
  public lineChartData: ChartDataSets[] = [{}];
  public lineChartLabels: Label[] = [];
  public lineChartOptions: (ChartOptions & { annotation: any }) = {
    responsive: true,
    maintainAspectRatio: false,
    scales: {
      // We use this empty structure as a placeholder for dynamic theming.
      xAxes: [{
        position: 'bottom',
        gridLines: {
          zeroLineColor: "rgba(0,255,0,1)"
        },
        scaleLabel: {
          display: true,
          labelString: 'x axis'
        },
        ticks: {
          suggestedMin: 0,
          suggestedMax: 100,
        }
      }],
      yAxes: [{
        position: 'left',
        gridLines: {
          zeroLineColor: "rgba(0,255,0,1)"
        },
        scaleLabel: {
          display: true,
          labelString: 'y axis'
        }
      }]
    },
    annotation: { },
  };
  public lineChartColors: Color[] = [
    { // grey
      backgroundColor: 'rgb(255, 99, 132, 0.2)', borderColor: 'rgb(255, 99, 132)',
      pointBackgroundColor: 'rgba(148,159,177,0.2)',
      pointBorderColor: '#fff',
      pointHoverBackgroundColor: '#fff',
      pointHoverBorderColor: 'rgba(148,159,177,0.8)'
    },
    { // dark grey
      backgroundColor: 'rgb(255, 99, 132, 0.2)', borderColor: 'rgb(255, 99, 132)',
      pointBackgroundColor: 'rgba(77,83,96,0.2)',
      pointBorderColor: '#fff',
      pointHoverBackgroundColor: '#fff',
      pointHoverBorderColor: 'rgba(77,83,96,1)'
    },
    { // grey
      backgroundColor: 'rgb(255, 99, 132, 0.2)', borderColor: 'rgb(255, 99, 132)',
      pointBackgroundColor: 'rgba(148,159,177,0.2)',
      pointBorderColor: '#fff',
      pointHoverBackgroundColor: '#fff',
      pointHoverBorderColor: 'rgba(148,159,177,0.8)'
    }
  ];
  public lineChartLegend = true;
  public lineChartType = 'line';

  @ViewChild(BaseChartDirective) chart: BaseChartDirective;

  constructor(public database: DatabaseService) { }

  ngOnInit() {
    this.database.ottieniRecordDivisi(undefined, 40).subscribe(record => {
      this.lineChartData = []
      this.lineChartLabels = []
      record.forEach(recordSingolo => {
        this.lineChartData.push({
          label: recordSingolo.idBarchetta.path,
          data: recordSingolo.record.map((el, index) => {
            return {
              y: el.altitudine,
              x: index
            }
          }),
        })
        
        if(this.lineChartLabels.length < recordSingolo.record.length)
          for(let i = 0; i < recordSingolo.record.length; i++) this.lineChartLabels.push(String(i))

        console.log('chart data', this.lineChartData)
      })
      this.chart.update()
    })
  }
}
