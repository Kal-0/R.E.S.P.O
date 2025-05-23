import { ApplicationConfig, provideZoneChangeDetection } from '@angular/core';
import { provideRouter } from '@angular/router';
import { routes } from './app.routes';

// Firebase
import { initializeApp, provideFirebaseApp } from '@angular/fire/app';
import { getFirestore, provideFirestore } from '@angular/fire/firestore';
import { getDatabase, provideDatabase } from '@angular/fire/database';
import { getAuth, provideAuth } from '@angular/fire/auth';
import { environment } from '../environments/environments';

export const appConfig: ApplicationConfig = {
  providers: [
    provideZoneChangeDetection({ eventCoalescing: true }),
    provideRouter(routes),
    
    // Initialize Firebase
    {
      provide: 'FIREBASE_DEBUG',
      useValue: (() => {
        console.log('=== Firebase Initialization ===');
        console.log('Initializing Firebase with config:', environment.firebaseConfig);
        return 'Firebase debug enabled';
      })()
    },
    
    // Initialize Firebase App
    provideFirebaseApp(() => {
      console.log('Initializing Firebase App...');
      const app = initializeApp(environment.firebaseConfig);
      console.log('Firebase App initialized');
      return app;
    }),
    
    // Initialize Firestore with explicit settings
    provideFirestore(() => {
      console.log('Initializing Firestore...');
      try {
        const firestore = getFirestore();
        console.log('Firestore initialized');
        return firestore;
      } catch (error) {
        console.error('Error initializing Firestore:', error);
        throw error; // Re-throw to prevent silent failures
      }
    }),
    
    // Initialize Realtime Database
    provideDatabase(() => {
      console.log('Initializing Realtime Database...');
      const database = getDatabase();
      console.log('Realtime Database initialized');
      return database;
    }),
    
    // Initialize Auth
    provideAuth(() => {
      console.log('Initializing Firebase Auth...');
      return getAuth();
    })
  ]
};
