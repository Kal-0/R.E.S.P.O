import { ComponentFixture, TestBed } from '@angular/core/testing';

import { LedRgbComponent } from './led-rgb.component';

describe('LedRgbComponent', () => {
  let component: LedRgbComponent;
  let fixture: ComponentFixture<LedRgbComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [LedRgbComponent]
    })
    .compileComponents();

    fixture = TestBed.createComponent(LedRgbComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
