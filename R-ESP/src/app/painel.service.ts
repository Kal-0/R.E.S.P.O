import { Injectable } from '@angular/core';
import { Firestore, collection, collectionData, doc, setDoc, updateDoc, deleteDoc, DocumentReference } from '@angular/fire/firestore';
import { Observable } from 'rxjs';

export interface DeviceData {
  id?: string;
  name: string;
  status: string;
  lastUpdated: Date;
  // Add more device properties as needed
}

@Injectable({
  providedIn: 'root'
})
export class PainelService {
  private readonly COLLECTION_NAME = 'devices';

  constructor(private firestore: Firestore) { }

  // Get all devices
  getDevices(): Observable<DeviceData[]> {
    const devicesRef = collection(this.firestore, this.COLLECTION_NAME);
    return collectionData(devicesRef, { idField: 'id' }) as Observable<DeviceData[]>;
  }

  // Get a single device
  async getDevice(id: string): Promise<DeviceData | null> {
    const deviceRef = doc(this.firestore, this.COLLECTION_NAME, id);
    // You'll need to implement this using getDoc if you need to fetch a single document
    // This is a simplified version
    return null;
  }

  // Add or update a device
  async saveDevice(device: DeviceData): Promise<void> {
    if (device.id) {
      // Update existing device
      const deviceRef = doc(this.firestore, this.COLLECTION_NAME, device.id);
      await updateDoc(deviceRef, { ...device });
    } else {
      // Add new device
      const newDeviceRef = doc(collection(this.firestore, this.COLLECTION_NAME));
      await setDoc(newDeviceRef, {
        ...device,
        id: newDeviceRef.id,
        lastUpdated: new Date()
      });
    }
  }

  // Delete a device
  async deleteDevice(id: string): Promise<void> {
    const deviceRef = doc(this.firestore, this.COLLECTION_NAME, id);
    await deleteDoc(deviceRef);
  }

  // Update device status
  async updateDeviceStatus(deviceId: string, status: string): Promise<void> {
    const deviceRef = doc(this.firestore, this.COLLECTION_NAME, deviceId);
    await updateDoc(deviceRef, {
      status,
      lastUpdated: new Date()
    });
  }
}
