import { Component, OnInit, OnDestroy, inject, NgZone } from '@angular/core';
import { RouterOutlet } from '@angular/router';
import { CommonModule } from '@angular/common';
import { Database, ref, set, onValue, off, push, serverTimestamp } from '@angular/fire/database';
import { Auth, User, onAuthStateChanged, signInAnonymously, Unsubscribe, AuthError } from '@angular/fire/auth';
import { Subscription } from 'rxjs';
import { environment } from '../environments/environments';

interface PotentiometerData {
  value: number;
  timestamp: number | object; // Can be number (millis) or server timestamp object
}

interface ConnectionTestData {
  timestamp: number | object; // Can be number (millis) or server timestamp object
  status: string;
  environment: {
    protocol: string;
    host: string;
    userAgent?: string;
  };
}

@Component({
  selector: 'app-root',
  standalone: true,
  imports: [RouterOutlet, CommonModule],
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.css']
})
export class AppComponent implements OnInit, OnDestroy {
  // Firebase services
  private readonly database: Database = inject(Database);
  private readonly auth: Auth = inject(Auth);
  
  // Subscriptions
  private readonly subscriptions: Subscription = new Subscription();
  private authUnsubscribe: Unsubscribe | null = null;
  private potListenerUnsubscribe: (() => void) | null = null;
  private readonly ngZone: NgZone = inject(NgZone);
  
  // Component state
  isConnected: boolean = false;
  isAuthenticated: boolean = false;
  connectionError: string | null = null;
  lastUpdate: Date | null = null;
  potentiometerValue: number | null = null;
  user: User | null = null;
  authError: string | null = null;
  
  // UI state
  isLoading: boolean = true;
  
  // Constants
  private readonly RETRY_DELAY = 5000; // 5 seconds
  private readonly MAX_RETRIES = 3;
  title = 'R-ESP';
  private retryCount = 0;

  ngOnInit() {
    this.initializeApp();
  }

  ngOnDestroy(): void {
    this.cleanupSubscriptions();
    if (this.authUnsubscribe) {
      this.authUnsubscribe();
      this.authUnsubscribe = null;
    }
  }
  
  private initializeApp(): void {
    // Set up authentication state listener
    this.authUnsubscribe = onAuthStateChanged(
      this.auth,
      (user) => this.ngZone.run(() => this.handleAuthStateChange(user)),
      (error) => this.ngZone.run(() => this.handleAuthError(error))
    );
  }
  
  private handleAuthStateChange(user: User | null): void {
    this.user = user;
    
    if (user) {
      console.log('✅ User authenticated:', user.uid);
      this.isAuthenticated = true;
      this.connectionError = null;
      this.authError = null;
      // Test Firebase connection after authentication
      this.testRealtimeConnection().catch(console.error);
    } else {
      console.log('🔐 No authenticated user - attempting anonymous sign-in');
      this.isAuthenticated = false;
      this.attemptAnonymousSignIn().catch(console.error);
    }
  }
  
  private handleAuthError(error: Error): void {
    console.error('🔥 Auth error:', error);
    this.isAuthenticated = false;
    this.authError = error.message || 'Authentication error';
    this.connectionError = this.authError;
    this.retryConnection();
  }
  
  private async attemptAnonymousSignIn(): Promise<void> {
    try {
      console.log('🔐 Attempting anonymous sign-in...');
      const userCredential = await signInAnonymously(this.auth);
      this.user = userCredential.user;
      this.isAuthenticated = true;
      this.authError = null;
      this.connectionError = null;
      console.log('✅ Anonymous authentication successful');
    } catch (error: unknown) {
      const errorMessage = error instanceof Error ? error.message : 'Unknown error';
      console.error('❌ Anonymous sign-in failed:', error);
      this.isAuthenticated = false;
      this.authError = `Authentication failed: ${errorMessage}`;
      this.connectionError = this.authError;
      this.retryConnection();
    }
  }

  private cleanupSubscriptions(): void {
    if (this.potListenerUnsubscribe) {
      this.potListenerUnsubscribe();
      this.potListenerUnsubscribe = null;
    }
  }
  
  private logEnvironmentInfo(): void {
    console.log('Environment:', {
      production: environment.production,
      location: window.location.href,
      protocol: window.location.protocol,
      host: window.location.host,
      userAgent: navigator.userAgent
    });
    
    // Log Firebase config (without sensitive info)
    const { apiKey, ...safeConfig } = environment.firebaseConfig;
    console.log('Firebase config:', {
      ...safeConfig,
      apiKey: apiKey ? `${apiKey.substring(0, 5)}...${apiKey.substring(apiKey.length - 4)}` : 'MISSING',
      hasApiKey: !!apiKey
    });
  }
  
  private verifyFirebaseConfig(): void {
    const { apiKey, projectId } = environment.firebaseConfig;
    
    // Test if running in browser
    if (typeof window === 'undefined') {
      throw new Error('Not running in a browser environment');
    }
    
    // Check if Firebase config is valid
    if (!apiKey || !projectId) {
      throw new Error('Invalid Firebase configuration: Missing required fields');
    }
    
    // Check for common configuration issues
    if (apiKey.includes('YOUR_') || projectId.includes('YOUR_')) {
      throw new Error('Firebase configuration contains placeholder values');
    }
  }
  
  private handleConnectionError(error: unknown): void {
    const errorMessage = error instanceof Error ? error.message : 'Unknown error';
    console.error('❌ Realtime Database connection failed:', error);
    
    // More specific error handling
    if (errorMessage.includes('permission-denied')) {
      this.connectionError = 'Permission denied. Check your Realtime Database rules.';
    } else if (errorMessage.includes('not-found')) {
      this.connectionError = 'Firebase project not found. Check your configuration.';
    } else if (errorMessage.includes('network-request-failed')) {
      this.connectionError = 'Network error. Check your internet connection.';
    } else {
      this.connectionError = `Connection error: ${errorMessage}`;
    }
    
    this.isConnected = false;
    console.error('Connection error details:', {
      error,
      isOnline: navigator.onLine,
      timestamp: new Date().toISOString()
    });
    
    // Only retry for certain types of errors
    if (!errorMessage.includes('permission-denied') && !errorMessage.includes('not-found')) {
      this.retryConnection();
    }
  }
  
  private retryConnection(delay = 5000) {
    console.log(`⏳ Retrying connection in ${delay}ms...`);
    setTimeout(() => {
      this.testRealtimeConnection().catch(console.error);
    }, delay);
  }
  
  private setupPotentiometerListener(): void {
    try {
      const potRef = ref(this.database, 'sensor/potenciometro');
      
      this.potListenerUnsubscribe = onValue(potRef, (snapshot) => {
        const value = snapshot.val();
        if (value !== null && value !== undefined) {
          this.ngZone.run(() => {
            this.potentiometerValue = Number(value);
            this.lastUpdate = new Date();
          });
        }
      }, (error) => {
        console.error('Error in potentiometer listener:', error);
      });
    } catch (error) {
      console.error('Failed to set up potentiometer listener:', error);
    }
  }

  private async testRealtimeConnection(): Promise<void> {
    console.log('=== Testing Realtime Database Connection ===');
    this.isConnected = false;
    this.connectionError = null;
    this.isLoading = true;
    
    const timeout = new Promise((_, reject) => 
      setTimeout(() => reject(new Error('Connection timeout after 10s')), 10000)
    );
    
    try {
      console.log('Checking Realtime Database availability...');
      await Promise.race([
        this.performRealtimeConnectionTest(),
        timeout
      ]);
      
      console.log('✅ Realtime Database connection successful');
      this.ngZone.run(() => {
        this.isConnected = true;
        this.connectionError = null;
      });
      
      // Set up listener after successful connection
      this.setupPotentiometerListener();
      
    } catch (error: unknown) {
      this.handleConnectionError(error);
    } finally {
      this.isLoading = false;
    }
  }
  
  private async performRealtimeConnectionTest(): Promise<void> {
    this.logEnvironmentInfo();
    this.verifyFirebaseConfig();
    this.cleanupSubscriptions();
    
    if (!navigator.onLine) {
      throw new Error('No network connection detected');
    }

    const testRef = ref(this.database, 'connection/test');
    const testData: ConnectionTestData = {
      timestamp: { '.sv': 'timestamp' }, // Server timestamp for Realtime Database
      status: 'connection_test',
      environment: {
        protocol: window.location.protocol,
        host: window.location.host,
        userAgent: navigator.userAgent
      }
    };
    
    console.log('Writing test data to Realtime Database...');
    await set(testRef, testData);
    console.log('Test data written successfully');
  }

  async adjustPotentiometer(change: number): Promise<void> {
    if (!this.isConnected) {
      console.error('Cannot adjust potentiometer: Not connected to Firebase');
      this.connectionError = 'Not connected to Firebase';
      return;
    }

    try {
      // Calculate new value with bounds checking (0-4095 typical for 12-bit ADC)
      const currentValue = this.potentiometerValue ?? 0;
      let newValue = currentValue + change;
      newValue = Math.max(0, Math.min(4095, newValue)); // Clamp between 0-4095

      const potRef = ref(this.database, 'sensor/potenciometro');
      
      await set(potRef, newValue);
      
      console.log(`✅ Adjusted potentiometer to ${newValue}`);
      this.connectionError = null;
      
      // Update local state
      this.potentiometerValue = newValue;
      this.lastUpdate = new Date();
      
    } catch (error: unknown) {
      const errorMessage = error instanceof Error ? error.message : 'Unknown error';
      console.error('❌ Error adjusting potentiometer:', error);
      this.connectionError = `Failed to update potentiometer: ${errorMessage}`;
      this.retryConnection();
    }
  }

  // Kept for backward compatibility
  async simulatePotentiometerChange(): Promise<void> {
    const randomValue = Math.floor(Math.random() * 4096);
    await this.adjustPotentiometer(randomValue - (this.potentiometerValue ?? 0));
  }
}
