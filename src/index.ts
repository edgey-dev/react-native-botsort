import { NitroModules } from 'react-native-nitro-modules'
import type { BoTSortTracker } from './specs/BoTSortTracker.nitro'

export const BoTSort =
  NitroModules.createHybridObject<BoTSortTracker>('BoTSortTracker')

export * from './specs/BoTSortTracker.nitro'
