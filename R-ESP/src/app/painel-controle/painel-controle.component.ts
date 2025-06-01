import { Component, inject, OnDestroy } from '@angular/core';
import { Database, ref, set, onValue, Unsubscribe } from '@angular/fire/database';
import { FormsModule } from '@angular/forms';
import { CommonModule, LocationStrategy } from '@angular/common';
import { LedRgbComponent } from '../led-rgb/led-rgb.component';

type ControlType = 'potenciometro' | 'temperatura' | 'frequencia' | 'controlMode' | 'vida' | 'motor'| 'blue' | 'green' | 'red' | 'lanterna';
type ButtonType = 'mais' | 'menos';
type NumericControlType = 'potenciometro' | 'temperatura' | 'frequencia'  | 'vida' | 'motor' | 'blue' | 'green' | 'red'| 'lanterna';


interface ControlState {
  value: number;
  min: number;
  max: number;
  step: number;
  path: string;
  unit?: string;
}

interface ControlStateBoolean {
  value: boolean;
  path: string;
  unit?: string;
}


@Component({
  selector: 'app-painel-controle',
  standalone: true,
  imports: [CommonModule, FormsModule, LedRgbComponent],
  templateUrl: './painel-controle.component.html',
  styleUrls: ['./painel-controle.component.css']
})
export class PainelControleComponent implements OnDestroy {
  private database: Database = inject(Database);
  private listeners: { [key: string]: Unsubscribe } = {};
  
  // UI State
  activeButton: { type: ControlType, dir: ButtonType } | null = null;
  corPainelRGB: string = '#000000';
  
  // Form controls
  potenciometroDigitado: number = 0;
  temperaturaDigitado: number = 0;
  frequenciaDigitado: number = 0;
  vidaDigitado: number = 0;
  motorDigitado: number = 0;
  lanternaDigitado: number = 0;
  
  // Controls configuration
  controls: { 
    potenciometro: ControlState,
    temperatura: ControlState,
    frequencia: ControlState,
    controlMode: ControlStateBoolean,
    vida: ControlState,
    motor: ControlState,
    blue: ControlState,
    green: ControlState,
    red: ControlState,
    lanterna: ControlState
   } = {
    potenciometro: { 
      value: 0, 
      min: 0, 
      max: 4095, 
      step: 100, 
      path: 'sensor/potenciometro',
      unit: ''
    },
    temperatura: { 
      value: 20, 
      min: 0, 
      max: 100, 
      step: 1, 
      path: 'sensor/temperatura',
      unit: '°C'
    },
    frequencia: { 
      value: 50, 
      min: 0, 
      max: 1000, 
      step: 10, 
      path: 'sensor/buzzer',
      unit: 'Hz'
    },
    controlMode: {
      value: false,
      path: 'control_mode'
    },
    vida: {
      value: 10,
      min: 0,
      max: 100,
      step: 10,
      path: 'sensor/vida'
    },
      motor: {
      value: 10,
      min: 0,
      max: 100,
      step: 10,
      path: 'sensor/servo',
      unit: '°'
    },
    blue: {
      value: 0,
      min: 0,
      max: 255,
      step: 1,
      path: 'sensor/led/blue'
    },
    green: {
      value: 0,
      min: 0,
      max: 255,
      step: 1,
      path: 'sensor/led/green'
    },
    red: {
      value: 0,
      min: 0,
      max: 255,
      step: 1,
      path: 'sensor/led/red'
    },
    lanterna: {
      value: 0,
      min: 0,
      max: 4095,
      step: 100,
      path: 'sensor/luminosidade'
    },
  };

  constructor() {
    this.setupListeners();
  }

  ngOnDestroy() {
    Object.values(this.listeners).forEach(unsubscribe => unsubscribe && unsubscribe());
  }

  private setupListeners(): void {
    Object.entries(this.controls).forEach(([key, control]) => {
      const controlRef = ref(this.database, control.path);
      
    this.listeners[key] = onValue(controlRef, (snapshot) => {
      const value = snapshot.val();
      if (value !== null && value !== undefined) {
        if (key === 'controlMode') {
          this.controls.controlMode.value = Boolean(value);
        } 
        else {
          this.controls[key as ControlType].value = Number(value);
        }
      }
    });

    });
  }

  async adjustValue(type: NumericControlType, change: number): Promise<void> {
    try {
      const control = this.controls[type] as ControlState; // garante tipo numérico

      let newValue = control.value + change;

      newValue = Math.max(control.min, Math.min(control.max, newValue));

      const valueRef = ref(this.database, control.path);
      await set(valueRef, newValue);

      console.log(`✅ ${type} adjusted to ${newValue}`);
    } catch (error) {
      console.error(`❌ Error adjusting ${type}:`, error);
    }
  }


  // Button event handlers
onButtonDown(type: ControlType, buttonType: ButtonType): void {
  if (!this.isNumericControl(type)) return;

  const change = buttonType === 'mais' 
    ? this.controls[type].step 
    : -this.controls[type].step;

  this.adjustValue(type, change);
}

private isNumericControl(type: ControlType): type is NumericControlType {
  return ['potenciometro', 'temperatura', 'frequencia', 'vida', 'motor','blue','green', 'red', 'lanterna'].includes(type);
}


  onButtonUp(type: ControlType, buttonType: ButtonType): void {
    if (this.activeButton?.type === type && this.activeButton?.dir === buttonType) {
      this.activeButton = null;
    }
  }

  // Special button handlers
 onSpecialButtonClick(type: 'controlMode'): void {


    const control = this.controls[type];
    let newValue: boolean | number;

  
    newValue = !control.value; // toggle boolean
    

    const valueRef = ref(this.database, control.path);
    set(valueRef, newValue)
      .then(() => console.log(`✅ ${type} set to ${newValue}`))
      .catch(error => console.error(`❌ Error setting ${type}:`, error));
  }


  // Form handlers
  onInputSubmit(type: 'potenciometro' | 'temperatura' | 'vida' | 'frequencia' | 'motor'|'lanterna'): void {
    const control = this.controls[type];

    const inputValuesMap = {
      potenciometro: this.potenciometroDigitado,
      temperatura: this.temperaturaDigitado,
      vida: this.vidaDigitado,
      frequencia: this.frequenciaDigitado,
      motor: this.motorDigitado,
      lanterna: this.lanternaDigitado,
    };

    const inputValue = inputValuesMap[type];
    
    if (inputValue === null || inputValue === undefined) return;
    
    const newValue = Math.max(control.min, Math.min(control.max, inputValue));
    const valueRef = ref(this.database, control.path);
    
    set(valueRef, newValue)
      .then(() => console.log(`✅ ${type} set to ${newValue}`))
      .catch(error => console.error(`❌ Error setting ${type}:`, error));
  }

  // RGB LED control
  atualizarCorRGB(cor: string): void {
    this.corPainelRGB = cor;
    // Converte o hex para RGB
    const r = parseInt(cor.slice(1, 3), 16);
    const g = parseInt(cor.slice(3, 5), 16);
    const b = parseInt(cor.slice(5, 7), 16);

    console.log(`Cor selecionada: ${cor}`);
    console.log(`R: ${r}, G: ${g}, B: ${b}`);

    const redRef = ref(this.database, this.controls.red.path);
    const greenRef = ref(this.database, this.controls.green.path);
    const blueRef = ref(this.database, this.controls.blue.path);

    // Envia para o Firebase
    set(redRef, r)
      .then(() => console.log('✅ Red atualizado'))
      .catch(err => console.error('❌ Erro ao atualizar Red:', err));

    set(greenRef, g)
      .then(() => console.log('✅ Green atualizado'))
      .catch(err => console.error('❌ Erro ao atualizar Green:', err));

    set(blueRef, b)
      .then(() => console.log('✅ Blue atualizado'))
      .catch(err => console.error('❌ Erro ao atualizar Blue:', err));
        
  }
}
