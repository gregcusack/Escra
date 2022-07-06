#!/bin/sh

# Create ConfigMap with Prometheus config
kubectl --namespace monitoring create configmap prometheus-core \
  --from-file=./deployment/prometheus.yaml \
  --output yaml \
    > ./deployment/configMap.yaml
# Workaround since `--namespace monitoring` from above is not preserved
echo "  namespace: monitoring" >> ./deployment/configMap.yaml

# Create one single manifest file
target="./manifests.yaml"
rm -f "$target" 
echo "# Derived from ./deployment/manifests/" >> "$target"
for file in $(find ./deployment/manifests -type f -name "*.yaml" | sort) ; do
  echo "---" >> "$target"
  cat "$file" >> "$target"
done

# kubectl apply manifests file
kubectl apply --filename "$target"