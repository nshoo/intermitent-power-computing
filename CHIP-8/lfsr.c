bool(a){
	if(a){
		return 1;
	}
	return 0;
}
main(){
	auto count = 50;
	auto reg = 0x9B;
	auto tap = 0;
	while(count){
		tap = bool(reg & 0x20) ^ bool(reg & 0x10);
		reg = reg >> 1;
		if(tap){
			reg |= 0x80;
		} else {
			reg &= (~0x80);
		}
		count -= 1;
		output(count);
	}
	output(reg);
}