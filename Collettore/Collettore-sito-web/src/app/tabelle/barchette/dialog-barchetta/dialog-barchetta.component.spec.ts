import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { DialogBarchettaComponent } from './dialog-barchetta.component';

describe('DialogBarchettaComponent', () => {
  let component: DialogBarchettaComponent;
  let fixture: ComponentFixture<DialogBarchettaComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ DialogBarchettaComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(DialogBarchettaComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
