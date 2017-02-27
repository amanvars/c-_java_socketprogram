import java.io.*;  
import java.net.*;  
import java.lang.Runnable;
import java.util.*;
import java.lang.Thread;



class Matrix_info
{
	 static int rowA,rowB,colA,colB;  // variables to save values of order
	 
}


class Print_matrix
{
	public void print(){
	int i,j;
	System.out.println("Received Matrix A");
	for(i=0;i<Matrix_info.rowA;i++)
	{
		for(j=0;j<Matrix_info.colA;j++)
		{
			System.out.print(Matrix_value.matrixA[i][j]+" ");
		}
		System.out.println();
	}

	System.out.println("Received Matrix B");
	for(i=0;i<Matrix_info.rowB;i++)
	{
		for(j=0;j<Matrix_info.colB;j++)
		{
			System.out.print(Matrix_value.matrixB[i][j]+" ");
		}
		System.out.println();
	}
	
	}

	public void print_result()
	{
		int i,j;
	System.out.println("Resulted Matrix C");
	for(i=0;i<Matrix_info.rowA;i++)
	{
		for(j=0;j<Matrix_info.colB;j++)
		{
			System.out.print(Matrix_mul.matrixC[i][j]+" ");
		}
		System.out.println();
	}
	}

}

class Send_matrix extends Thread
{
	Socket sock_send;
	int ra ;
	int cb = Matrix_info.colB;

	public Send_matrix(Socket s,int row)
	{
		this.sock_send = s;
		this.ra = row;
	}

	public void run()
	{
		try{

			DataOutputStream dos = new DataOutputStream(sock_send.getOutputStream());
			for(int i=0; i<cb;i++)
			{
				dos.writeInt(Matrix_mul.matrixC[ra][i]);
			}
			
		}catch(Exception e)
		{
			System.out.println(e);
		}
	}
}

class Matrix_mul implements Runnable
{
	int matA[][];
	int matB[][];
	static int matrixC[][];
	int ra = Matrix_info.rowA;
	int ca = Matrix_info.colA;
	int rb = Matrix_info.rowB;
	int cb = Matrix_info.colB;
	int row,col;
	Socket sock_mul;

	public Matrix_mul(Socket s, int row, int col,int A[][],int B[][],int C[][])
	{
		this.sock_mul = s;
		this.row = row;  // setting current index of row and col;
		this.col = col;
		this.matA = A;
		this.matB = B;
		this.matrixC = C;

	}

	public void run()
	{
		try
		{
			int i,j;
			int sum = 0;
			

			for(i=0;i<ca;i++)
			{
				sum = sum + (matA[row][i]*matB[i][col]);
			}
			matrixC[row][col] = sum;

		}catch(Exception e)
		{
			System.out.println(e);
		}
	}
}

class Matrix_value extends Thread
{
	Socket socket_mat_value;
	int ra,ca,rb,cb;
	int mat_check_get=0;
	int i,j,k;
	public static int matrixA[][];
	public static int matrixB[][];
	public Matrix_value(Socket s)
	{
		this.socket_mat_value = s;
	}

	public void get_Matrix_Value()
	{
		Matrix_info mi_order = new Matrix_info();
		this.ra = mi_order.rowA;
		this.ca = mi_order.colA;
		this.rb = mi_order.rowB;
		this.cb = mi_order.colB;
	}

	public void run()
	{
		try
		{
			matrixA = new int[ra][ca];
			matrixB = new int[rb][cb];

			DataInputStream dis=new DataInputStream(socket_mat_value.getInputStream());  
			//int data;
			for(k=0;k<ra;k++){
				//rowa.clear();
				for(i=0;i<ca;i++)
				{
					int data = dis.readInt();
					System.out.println("Setting data in rowA at "+"["+k+"]["+i+"] : "+data);
					this.matrixA[k][i] = data;
					Thread.sleep(1000);
				}

					for(i=0;i<rb;i++)
					{
						//rowb.clear();
						for(j=0;j<cb;j++)
						{
							int data = dis.readInt();
							System.out.println("Setting data in MatrixB at "+"["+i+"]["+j+"] : "+data);
							this.matrixB[i][j] = data;
							Thread.sleep(1000);
						}
						
					}	
					System.out.println();
		}

		}catch(Exception e)
		{
			System.out.println(e);
		}
	}
}

class Mat_thread extends Thread
{
	int rA,rB,cA,cB;
	Socket socket_mat;
	public Mat_thread(Socket s)
	{
		this.socket_mat = s;
	}

	public Matrix_info get_Info()
	{
		Matrix_info mi = new Matrix_info();
		mi.rowA = rA;
		mi.colA = cA;
		mi.rowB = rB;
		mi.colB = cB;
		return mi; 	
	}

	public void run()
	{
		try
		{
			DataInputStream dis=new DataInputStream(socket_mat.getInputStream());  
			int data;
			data = dis.readInt();
			this.rA = data;
			System.out.println("Row Size A = "+rA);
			Thread.sleep(1000);
			data = dis.readInt();
			this.cA = data;
			System.out.println("Col Size A = "+cA);
			Thread.sleep(1000);
			data = dis.readInt();
			this.rB = data;
			System.out.println("Row Size B = "+rB);
			Thread.sleep(1000);
			data = dis.readInt();
			this.cB = data;
			System.out.println("Col Size B = "+cB);
			Thread.sleep(1000);


		}catch(Exception e)
		{
			System.out.println(e);
		}
	}

}



public class Server
 {  
	public static void main(String[] args)
	{  
	try{  
	ServerSocket ss=new ServerSocket(13000);  
	System.out.println("Server is Running.......\nStart Client."); 
	Socket s=ss.accept();//establishes connection  
	System.out.println("Connection Established with Client\n");

	DataInputStream dis=new DataInputStream(s.getInputStream());   // to get data from client
 	DataOutputStream dos = new DataOutputStream(s.getOutputStream());
	

	String inputLine; 
	BufferedReader in = new BufferedReader(new InputStreamReader(s.getInputStream())); 

	// Thread Making
    Mat_thread mt = new Mat_thread(s);
    mt.start();
    mt.join();

    Matrix_info mi_order = new Matrix_info();
    mi_order = mt.get_Info();


    System.out.println("Successfully got the Order of Matrices\n");

		int ra = mi_order.rowA;
		int ca = mi_order.colA;
		int rb = mi_order.rowB;
		int cb = mi_order.colB;

   	Matrix_value mv = new Matrix_value(s);
    mv.get_Matrix_Value();
    mv.start();
    mv.join();
    
    Print_matrix pm = new Print_matrix();
    pm.print();
    
    Send_matrix sm;

    int num_threads = ra*cb;
    // initializing thread array for computing row and column wise;

    Thread[] mat_mul = new Thread[num_threads];
    int thrd_cnt=0;

    int matA[][] = new int[ra][ca];
    int matB[][] = new int[rb][cb];

    // resulting matrix initialization
    int matC[][] = new int[ra][cb];

    // Assigning matrix A,B
    matA  = Matrix_value.matrixA;
    matB  = Matrix_value.matrixB;
   
   System.out.println("Doing Computation...");
     
           for(int row = 0 ; row < ra; row++)
           {
                for (int col = 0 ; col <cb ; col++ )
                {
                        // creating thread for multiplications
                     mat_mul[thrd_cnt] = new Thread(new Matrix_mul(s,row,col,matA,matB,matC));
                     mat_mul[thrd_cnt].start(); //thread start
                     
                     mat_mul[thrd_cnt].join(); // joining threads
                     thrd_cnt++;
                }
                System.out.println("Sending back Computed Row");
                Thread.sleep(1000);
                sm = new Send_matrix(s,row);
                sm.start();
                sm.join();
                
           }
          
        // printing the resulted matrix;
        
        pm.print_result();
		ss.close();  
		s.close();
}catch(Exception e){System.out.println(e);}  
}  
}  
