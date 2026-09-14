import { useEffect } from 'react';
import { Text, View, StyleSheet } from 'react-native';
import { BoTSort } from 'react-native-botsort';

export default function App() {
  useEffect(() => {
    BoTSort.initialize('', false);
  }, []);

  return (
    <View style={styles.container}>
      <Text>Result: help</Text>
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    alignItems: 'center',
    justifyContent: 'center',
  },
});
