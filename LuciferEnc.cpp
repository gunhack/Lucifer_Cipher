#include <iostream>

using namespace std;

class Lucifer {

public:

	void setKey(string key) {

		int c;

		if(key.size() < 16) {

			for(int i = key.size() ; i < 16; i++){

				key[i] = '\0';
			}
		}

		for(int i = 0; i < 16; i++) {

			c = key[i] & 0xFF;

			for(int j = 0; j < BPB; j++) {

				k[(BPB * i) + j] = c & 0x1;
				c = c >> 1;
			}
		}
	}

	string callLucifer(string msg, bool direction) {

		int counter = 0, output;
		char c;
		string buff = "";

		for(int i = 0; i < msg.size(); i++) {

			if(counter == 16) {

				lucifer(direction);

				for(counter = 0; counter < 16; counter++) {

					output = 0;

					for(int j = BPB - 1; j >= 0; j--) {

						output = (output << 1) + m[(BPB * counter) + j];
					}
					buff += (const char*) &output;
				}
				counter = 0;
			}

			c = msg[i];
			for(int j = 0; j < BPB; j++ ){

				m[(BPB * counter) + j] = c & 0x1;
				c = c >> 1;
			}

			counter += 1;
		}

		for( ; counter < 16; counter++) {

			for(int i = 0; i < BPB; i++) {

				m[(BPB * counter) + i] = 0;
			}
		}

		lucifer(direction);

		for(counter = 0; counter < 16; counter++) {

			output = 0;

			for(int i = BPB - 1; i >= 0; i--) {

				output = (output << 1) + m[(BPB * counter) + i];
			}

			buff += (const char*) &output;
		}

		return buff;
	}

	void lucifer(bool direction) {

		int tcbindex;
		int tcbcontrol  = (direction == DECIPHER ? 8 : 0);
		int hi, lo, h_0 = 0, h_1 = 1;
		int bit, temp1;
		int byte, index, v, tr[BPB];

		for(int round = 0; round < 16; round++) {

			if(direction == DECIPHER) {
				tcbcontrol = (tcbcontrol + 1) & 0xF;
			}

			tcbindex = tcbcontrol;

			for(byte = 0; byte < 8; byte++) {

				lo = (m[(h_1 * 64) + (BPB * byte) + 7]) * 8
					+(m[(h_1 * 64) + (BPB * byte) + 6]) * 4
					+(m[(h_1 * 64) + (BPB * byte) + 5]) * 2
					+(m[(h_1 * 64) + (BPB * byte) + 4]);

				hi = (m[(h_1 * 64) + (BPB * byte) + 3]) * 8
					+(m[(h_1 * 64) + (BPB * byte) + 2]) * 4
					+(m[(h_1 * 64) + (BPB * byte) + 1]) * 2
					+(m[(h_1 * 64) + (BPB * byte) + 0]);

				v = (s0[lo] + 16 * s1[hi]) * (1 - k[(BPB * tcbindex) + byte])
				   +(s0[hi] + 16 * s1[lo]) * k[(BPB * tcbindex) + byte];

				for(temp1 = 0; temp1 < BPB; temp1++) {

					tr[temp1] = v & 0x1;
					v = v >> 1;
				}

				for(bit = 0; bit < BPB; bit++){

					index = (o[bit] + byte) & 0x7;
					temp1 = m[(h_0 * 64) + (BPB * index) + bit]
						   +k[(BPB * tcbcontrol) + pr[bit]]
						   +tr[pr[bit]];
					m[(h_0 * 64) + (BPB * index) + bit] = temp1 & 0x1;
				}

				if( byte < 7 || direction == DECIPHER ) {
					tcbcontrol = (tcbcontrol + 1) & 0xF;
				}
			}

			temp1 = h_0;
			h_0 = h_1;
			h_1 = temp1;
		}

		for(byte = 0; byte < 8; byte++) {

			for(bit = 0; bit < BPB; bit++) {

				temp1 = m[(BPB * byte) + bit];
				m[(BPB * byte) + bit] = m[64 + (BPB * byte) + bit];
				m[64 + (BPB * byte) + bit] = temp1;
			}
		}
	}


	static const bool ENCIPHER = 0;
	static const bool DECIPHER = 1;

private:

	static const unsigned short int L_BLOCK = 128;				/* bits in a lucifer block */
	static const unsigned short int BPB = 8;					/* bits per byte */

	int m[L_BLOCK];												/* message vector */
	int k[L_BLOCK];												/* key vector */

	int o[8] = {7, 6, 2, 1, 5, 0, 3, 4};					/* diffusion pattern */
	int pr[8] = {2, 5, 4, 0, 3, 1, 7, 6};					/* inverse of fixed permutation */

	/* S-box permutations */
	int s0[16] = {12, 15, 7, 10, 14, 13, 11, 0, 2, 6, 3, 1, 9, 4, 5, 8};
	int s1[16] = {7, 2, 14, 9, 3, 11, 0, 4, 12, 13, 1, 10, 6, 15, 8, 5};
};

int main() {

	Lucifer L;
	L.setKey("pasword");

	string t = L.callLucifer("Message to encrypt", L.ENCIPHER);
	cout << t << endl;

	cout << L.callLucifer(t, L.DECIPHER);

	return 0;
}
