
#define VERSION_NUMBER 1

struct train {
	int trainNo;
	int seatsAvlb;
};

%extern train bookingInfo[100];


program SIMP_PROG {
   version SIMP_VERSION {
     string lookUp(int) = 1;
     string bookTkt(int) = 2;
   } = VERSION_NUMBER;
} = 555555555;
