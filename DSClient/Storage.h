/************************************************************************
 ** This file is part of the network simulator Shawn.                  **
 ** Copyright (C) 2004-2007 by the SwarmNet (www.swarmnet.de) project  **
 ** Shawn is free software; you can redistribute it and/or modify it   **
 ** under the terms of the BSD License. Refer to the shawn-licence.txt **
 ** file in the root of the Shawn source tree for further details.     **
 ************************************************************************
 **                                                                    **
 ** \author Axel Wegener <wegener@itm.uni-luebeck.de>                  **
 **                                                                    **
 ************************************************************************/
#include <vector>
#include <string>
#include <stdexcept>

using namespace std;


class Storage
{

    public:
        typedef std::vector<unsigned char> StorageType;

    private:
        StorageType store;

        unsigned int pos_;
        bool iterValid_;
        bool iterEndValid_;

        StorageType::const_iterator iter_;
        StorageType::const_iterator iterEnd_;

        // sortation of bytes forwards or backwards?
        bool bigEndian_;

        /// Used in constructors to initialize local variables
        void init();

    public:
        Storage();

        /// Constructor, that fills the storage with an char array. If length is -1, the whole array is handed over
        Storage(unsigned char[], int length=-1);

        // Destructor
        virtual ~Storage();

        virtual bool valid_pos();
        virtual unsigned int position() const;

        void reset();

        virtual unsigned char readChar() throw(std::invalid_argument);
        virtual void writeChar(unsigned char) throw();

        virtual int readByte() throw();
        virtual void writeByte(int) throw(std::invalid_argument);

        virtual int readUnsignedByte() throw();
        virtual void writeUnsignedByte(int) throw(std::invalid_argument);

        virtual std::string readString() throw();
        virtual void writeString(std::string s) throw();

        virtual int readShort() throw();
        virtual void writeShort(int) throw(std::invalid_argument);

        virtual int readInt() throw();
        virtual void writeInt(int) throw();

        virtual float readFloat() throw();
        virtual void writeFloat( float ) throw();

        virtual double readDouble() throw();
        virtual void writeDouble( double ) throw();

        virtual void writePacket(unsigned char* packet, int length);

        virtual void writeStorage(Storage& store);

        // Some enabled functions of the underlying std::list
        int size() const { return store.size(); }

        StorageType::const_iterator begin() const { return store.begin(); }
        StorageType::const_iterator end() const { return store.end(); }

};
