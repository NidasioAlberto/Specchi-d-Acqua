import { Component, OnInit, Input, ViewChild, ElementRef, Output, EventEmitter } from '@angular/core';
import { ResizedEvent } from 'angular-resize-event';
import { fromEvent, interval } from 'rxjs';
import { debounceTime, debounce } from 'rxjs/operators';

@Component({
  selector: 'app-joystick',
  templateUrl: './joystick.component.html',
  styleUrls: ['./joystick.component.scss']
})
export class JoystickComponent implements OnInit {

  @Input() width: number
  @Input() height: number

  @Output() updateControls = new EventEmitter<{direction: number, speed: number}>()

  @ViewChild('joystick', { static: true }) 
  canvas: ElementRef<HTMLCanvasElement>
  ctx: CanvasRenderingContext2D

  mouseDown: boolean = false

  maxRaggio: number = 200
  velocita: number = 0

  constructor() { }

  ngOnInit() {
    this.ctx = this.canvas.nativeElement.getContext('2d')
    
    this.canvas.nativeElement.onmousedown = (event) => {
      this.mouseDown = true
      this.disegnaJoystick(event.clientX, event.clientY)
    }

    this.canvas.nativeElement.ontouchstart = (event) => {
      this.mouseDown = true
      this.disegnaJoystick(event.touches[0].clientX, event.touches[0].clientY)
    }

    this.canvas.nativeElement.onmousemove = (event) => {
      if (this.mouseDown) this.disegnaJoystick(event.clientX, event.clientY)
    }

    this.canvas.nativeElement.ontouchmove = (event) => {
      if (this.mouseDown) this.disegnaJoystick(event.touches[0].clientX, event.touches[0].clientY)
    }

    this.canvas.nativeElement.onmouseup = (event) => {
      this.mouseDown = false
      this.disegnaJoystick(this.width/2, this.height/2)
    }

    this.canvas.nativeElement.ontouchend = (event) => {
      this.mouseDown = false
      this.disegnaJoystick(this.width/2, this.height/2)
    }
  }
 
  onResized(event: ResizedEvent) {
    this.width = event.newWidth
    this.height = event.newHeight

    this.ctx.canvas.width = this.width
    this.ctx.canvas.height = this.height

    this.maxRaggio = (this.width < this.height ? this.width : this.height) * (3/9);

    this.disegnaJoystick(this.width/2, this.height/2, false)
  }

  disegnaJoystick(xUtente: number, yUtente: number, needUpdate: boolean = true) {
    // Pulisco la canvas
    this.ctx.clearRect(0, 0, this.ctx.canvas.width, this.ctx.canvas.height)
    this.ctx.fillStyle = 'white'
    this.ctx.fillRect(0, 0, this.ctx.canvas.width, this.ctx.canvas.height)

    // Calcolo la posizione dove il joystick deve essere disegnato
    let xUtenteRelativo = xUtente - this.width/2
    let yUtenteRelativo = yUtente - this.height/2

    let raggio = Math.sqrt(Math.pow((Math.abs(xUtente - this.width/2)), 2) + Math.pow((Math.abs(yUtente - this.height/2)), 2))
    let teta = Math.atan2(xUtenteRelativo, yUtenteRelativo)

    this.velocita = raggio/this.maxRaggio

    let speed = (raggio / this.maxRaggio) * 100
    if (speed > 100) speed = 100

    if(needUpdate) this.updateControls.emit({direction: teta, speed})

    if(raggio > this.maxRaggio) raggio = this.maxRaggio

    let xJoystick = this.width/2 + Math.sin(teta) * raggio
    let yJoystick = this.height/2 + Math.cos(teta) * raggio

    //Disegno il cerchio del joystick alle coordinare specificate
    this.ctx.fillStyle = '#42a5f5'
    this.ctx.beginPath()
    this.ctx.arc(xJoystick, yJoystick, 50, 0, 360)
    this.ctx.fill()
    this.ctx.closePath()
  }
}
