#!/bin/sh

# Create ConfigMap with Prometheus config
kubectl --namespace monitoring create configmap prometheus-core \
  --from-file=./prometheus/prometheus.yaml \
  --output yaml \
    > ./prometheus/configMap.yaml
# Workaround since `--namespace monitoring` from above is not preserved
echo "  namespace: monitoring" >> ./prometheus/configMap.yaml

# Create one single manifest file
target="./manifests.yaml"
rm -f "$target" 
echo "# Derived from ./prometheus/manifests/" >> "$target"
for file in $(find ./prometheus/manifests -type f -name "*.yaml" | sort) ; do
  echo "---" >> "$target"
  cat "$file" >> "$target"
done

# kubectl apply manifests file
kubectl apply --filename "$target"