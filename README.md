# react-native-botsort

This project ports the popular [BoT-SORT](https://github.com/NirAharon/BoT-SORT) multi-object tracking algorithm to mobile. Its native C++ implementation is exposed to React Native through [Nitro Modules](https://nitro.margelo.com/) and integrates with [VisionCamera](https://github.com/mrousavy/react-native-vision-camera).

- Native C++ tracking for iOS and Android
- VisionCamera `Frame` input
- ORB, ECC, and sparse optical flow global motion compensation

## Installation

1. Install the library and its native dependencies:

  ```sh
  npm install react-native-botsort react-native-nitro-modules react-native-vision-camera react-native-nitro-image
  ```

1. Install iOS pods:

  ```sh
  npx pod-install
  ```

1. Follow the [VisionCamera setup guide](https://visioncamera.margelo.com/docs) for camera permissions and native configuration, then rebuild your app.

For the `Frame` integration, use a Nitro-enabled VisionCamera version. The example app uses VisionCamera 5.2.3. Android builds require API level 26 or later.

## Usage

Initialize the tracker, then call `track` from your VisionCamera frame-processing flow with the detections for that frame:

```ts
import {
  botsort,
  GMCMethod,
  type BoundingBox,
  type TrackedObject,
} from 'react-native-botsort';
import type { Frame } from 'react-native-vision-camera';

botsort.initialize(
  {
    enable_gmc: true,
    frame_rate: 30,
  },
  {
    gmc_method: GMCMethod.ORB,
  },
);

function processFrame(frame: Frame, detections: BoundingBox[]): TrackedObject[] {
  return botsort.track(frame, detections);
}
```

Each detection uses pixel coordinates relative to the top-left of the frame:

```ts
const detections: BoundingBox[] = [
  {
    x: 120,
    y: 80,
    width: 64,
    height: 112,
    confidence: 0.92,
    classId: 0,
  },
];
```

Pass an empty array when there are no detections. Each result contains a tracker ID, box, and class ID:

```ts
const tracks: TrackedObject[] = processFrame(frame, detections);
// Each result has: id, x, y, w, h, classId
```

> [!NOTE]
> This implementation currently doesn't support or implement apperance based reid.

## Configuration

All fields in `BoTSORTConfig` are optional. The defaults are:

| Field | Default | Description |
| --- | ---: | --- |
| `enable_gmc` | `false` | Enable global camera-motion compensation. |
| `track_high_thresh` | `0.6` | Confidence threshold for high-confidence detections. |
| `track_low_thresh` | `0.1` | Minimum confidence used for low-confidence association. |
| `new_track_thresh` | `0.7` | Minimum confidence for starting a new track. |
| `track_buffer` | `30` | Lost-track retention, in nominal frames at 30 FPS. |
| `match_thresh` | `0.7` | Association cost threshold. |
| `proximity_thresh` | `0.5` | Proximity/IoU-distance threshold used during association. |
| `frame_rate` | `30` | Expected input frame rate; used to scale lost-track retention. |

To enable GMC, set `enable_gmc: true` and pass a matching GMC configuration as the second argument. If GMC is enabled without a configuration, the native implementation disables GMC.

```ts
// ORB feature matching
botsort.initialize(
  { enable_gmc: true },
  { gmc_method: GMCMethod.ORB, downscale: 2 },
);

// ECC image alignment
botsort.initialize(
  { enable_gmc: true },
  { gmc_method: GMCMethod.ECC, downscale: 5 },
);

// Sparse optical flow
botsort.initialize(
  { enable_gmc: true },
  { gmc_method: GMCMethod.SOF, downscale: 2 },
);
```

The exported `GMCConfig` union provides these method-specific options:

- `OrbGMCConfig`: `downscale`, `inlier_ratio`, `ransac_conf`, `ransac_max_iters`.
- `EccGMCConfig`: `downscale`, `max_iterations`, `termination_eps`.
- `SofGMCConfig`: `downscale`, `use_harris_detector`, `max_corners`, `block_size`, `quality_level`, `k`, `min_distance`, `inlier_ratio`, `ransac_conf`, `ransac_max_iters`.

TypeScript declarations include the defaults and types for each option. The older `gmc_method` field inside `BoTSORTConfig` is deprecated; select a method in the second `initialize` argument instead.

## API

### `botsort.initialize(trackerConfig, gmcConfig?)`

Creates or resets the native tracker. `trackerConfig` is a `BoTSORTConfig`; `gmcConfig` is an optional `OrbGMCConfig`, `EccGMCConfig`, or `SofGMCConfig`.

### `botsort.track(frame, detections)`

Updates the tracker with a VisionCamera `Frame` and an array of `BoundingBox` detections, then returns `TrackedObject[]`.

### `BoundingBox`

| Field | Type | Description |
| --- | --- | --- |
| `x`, `y` | `number` | Top-left position in frame pixels. |
| `width`, `height` | `number` | Box dimensions in frame pixels. |
| `confidence` | `number` | Detector confidence score. |
| `classId` | `number` | Detector class identifier. |

### `TrackedObject`

| Field | Type | Description |
| --- | --- | --- |
| `id` | `number` | Tracker-assigned identity. |
| `x`, `y` | `number` | Top-left position in frame pixels. |
| `w`, `h` | `number` | Tracked box dimensions in pixels. |
| `classId` | `number` | Associated detector class identifier. |

## Development

This repository is a Yarn workspace containing the library and an example app. From the repository root:

```sh
yarn
yarn nitrogen
yarn typecheck
yarn lint
```

Run Metro, launch the Android example, or build the iOS example for the generic device destination with:

```sh
yarn example start
yarn example android
yarn workspace react-native-botsort-example build:ios
```

The example app is the native integration target. See [CONTRIBUTING.md](CONTRIBUTING.md) for the development workflow and pull-request guidance.

## License

MIT. See [LICENSE](LICENSE).
