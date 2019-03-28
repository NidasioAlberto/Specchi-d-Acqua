import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { ImpostazioniGraficiComponent } from './impostazioni-grafici.component';

describe('ImpostazioniGraficiComponent', () => {
  let component: ImpostazioniGraficiComponent;
  let fixture: ComponentFixture<ImpostazioniGraficiComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ ImpostazioniGraficiComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(ImpostazioniGraficiComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
