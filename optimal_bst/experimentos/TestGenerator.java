public class TestGenerator
{
		public static void main(String[] args)
		{
			int n = Integer.parseInt(args[0]);
			System.out.println(n);
			for (int i = 0; i < n; ++i){
				double r = Math.random() * 100;
				System.out.println(r);	
			}
		}

};
