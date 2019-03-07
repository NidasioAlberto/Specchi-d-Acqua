import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { TabelleComponent } from './tabelle.component';

describe('TabelleComponent', () => {
  let component: TabelleComponent;
  let fixture: ComponentFixture<TabelleComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ TabelleComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(TabelleComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
