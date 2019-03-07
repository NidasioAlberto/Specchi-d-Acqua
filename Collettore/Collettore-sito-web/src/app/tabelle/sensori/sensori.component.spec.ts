import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { SensoriComponent } from './sensori.component';

describe('SensoriComponent', () => {
  let component: SensoriComponent;
  let fixture: ComponentFixture<SensoriComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ SensoriComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(SensoriComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
