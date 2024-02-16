#include "../nyx/nn.h"

int main() {
	sda::SDA<int> layerSizes(4);
	layerSizes.set(0, 2);
	layerSizes.set(1, 3);
	layerSizes.set(2, 3);
	layerSizes.set(3, 1);

	nn::FNN fnn(layerSizes, 0.01);

	sda::SDA<double> ia(2);
	ia.set(0, 0);
	ia.set(1, 0);
	sda::SDA<double> oa(1);
	oa.set(0, 0);

	sda::SDA<double> ib(2);
	ib.set(0, 1);
	ib.set(1, 0);
	sda::SDA<double> ob(1);
	ob.set(0, 1);

	sda::SDA<double> ic(2);
	ic.set(0, 0);
	ic.set(1, 1);
	sda::SDA<double> oc(1);
	oc.set(0, 1);
	
	sda::SDA<double> id(2);
	id.set(0, 1);
	id.set(1, 1);
	sda::SDA<double> od(1);
	od.set(0, 0);
	
	sda::SDA<sda::SDA<double>> inputs(4);
	sda::SDA<sda::SDA<double>> outputs(4);
	inputs.set(0, ia);
	inputs.set(1, ib);
	inputs.set(2, ic);
	inputs.set(3, id);
	outputs.set(0, oa);
	outputs.set(1, ob);
	outputs.set(2, oc);
	outputs.set(3, od);

	fnn.train(inputs, outputs, 100);
}