# react-native-botsort

A react native library created with Nitro that implements a stripped down BoTSORT tracking algorthim made for mobile inference

## Installation

```sh
npm install react-native-botsort react-native-nitro-modules

> `react-native-nitro-modules` is required as this library relies on [Nitro Modules](https://nitro.margelo.com/).
```

## Usage

```js
import { botsort, GMCMethod } from 'react-native-botsort';

// ...

botsort.initialize({ enable_gmc: true}, { gmc_method: GMCMethod::SOF});

botsort.track(frame, detectionBox)
```

## Contributing

- [Development workflow](CONTRIBUTING.md#development-workflow)
- [Sending a pull request](CONTRIBUTING.md#sending-a-pull-request)
- [Code of conduct](CODE_OF_CONDUCT.md)

## License

MIT

---

Made with [create-react-native-library](https://github.com/callstack/react-native-builder-bob)
