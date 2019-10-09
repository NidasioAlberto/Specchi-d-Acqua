import { Component, OnInit, Input, ViewChild, ElementRef } from '@angular/core';
import { ResizedEvent } from 'angular-resize-event';

@Component({
  selector: 'app-joystick',
  templateUrl: './joystick.component.html',
  styleUrls: ['./joystick.component.scss']
})
export class JoystickComponent implements OnInit {

  @Input() width: number
  @Input() height: number

  @ViewChild('joystick', { static: true }) 
  canvas: ElementRef<HTMLCanvasElement>
  ctx: CanvasRenderingContext2D

  constructor() { }

  ngOnInit() {
    this.ctx = this.canvas.nativeElement.getContext('2d')
  }
 
  onResized(event: ResizedEvent) {
    this.width = event.newWidth
    this.height = event.newHeight

    this.ctx.canvas.width = this.width
    this.ctx.canvas.height = this.height

    this.ctx.fillStyle = 'white'
    this.ctx.fillRect(0, 0, this.ctx.canvas.width, this.ctx.canvas.height)
    this.ctx.fillStyle = 'red'
    this.ctx.fillRect(200, 200, this.ctx.canvas.width/2, 700)
  }
}
