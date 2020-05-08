class TestJ{
	enum ENumType{
		EOne(10),
		ETwo(20),
		EThree(30);
		private int m;
		ENumType(int x){
		    m = x;
		}
	};
	public static void main(String[] args){
		ENumType t = ENumType.EThree;
		ENumType o = ENumType.EOne;
		ENumType eo = ENumType.EOne;
		ENumType et = ENumType.ETwo;
		System.out.println("t==ENumType.EThree is "+ (t==ENumType.EThree) );
		System.out.println("o==t is "+ (o==t) );
		System.out.println("o==eo is "+ (o==eo) );
		System.out.println("et= "+ et );
	}
};
