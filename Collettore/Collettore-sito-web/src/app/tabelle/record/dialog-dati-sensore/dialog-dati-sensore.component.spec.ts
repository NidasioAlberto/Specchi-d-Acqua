import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { DialogDatiSensoreComponent } from './dialog-dati-sensore.component';

describe('DialogDatiSensoreComponent', () => {
  let component: DialogDatiSensoreComponent;
  let fixture: ComponentFixture<DialogDatiSensoreComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ DialogDatiSensoreComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(DialogDatiSensoreComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
