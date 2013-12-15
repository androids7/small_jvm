/*
 * JvmExecuter.h
 *
 *  Created on: Sep 8, 2012
 *      Author: inti
 */

#ifndef JVMEXECUTER_H_
#define JVMEXECUTER_H_

#include <map>
#include <vector>
#include <functional>

#include "../jvmclassfile/classfile.h"
#include "../jvmclassfile/classloader.h"

#include "../mm/Clase.h"
#include "../mm/Space.h"

#include "jni.h"

using namespace std;

using namespace MemoryManagement;

void initJNI();

namespace jvm {

	extern JNIEnv* env;

	class JvmExecuter {
		protected:
			ClassLoader* loader;
			Space* space;
			map<string, Type*> classes;

			vector<void*> loadedLibraries;
			map<string, void*> nativeMethods;

			int countOfClassObjects;
			Objeto classObjects[1000];
			map<string, int> metaclasses;

			virtual void initiateClass(ClassFile* cf) = 0;

			ClassFile* loadAndInit(string class_name);
		public:
			JvmExecuter(ClassLoader* loader, Space* space);
			virtual ~JvmExecuter();

			virtual void execute(ClassFile* cf, MethodInfo* method, std::function<void(JvmExecuter*, void* addr)> fn) = 0;

			static void execute(ClassFile* cf, const char* method, const char* description,
					JvmExecuter* exec, std::function<void(JvmExecuter*, void* addr)> fn) {
				int16_t index = cf->getCompatibleMethodIndex(method, description);
				if (index < 0 || index >= cf->methods_count)
					throw new exception();

				MethodInfo* mi = cf->methods[index];

				if ((mi->access_flags & ACC_STATIC)) {
					exec->execute(cf, mi, fn);
				} else
					throw new exception();
			}

			static int countParameters(string s);

			Type* getType(string javaDescription);
			Type* buildInMemoryClass(ClassFile* cf);

			void callStaticNativeMethod(string signature, Clase* clazz);
	};

} /* namespace jvm */
#endif /* JVMEXECUTER_H_ */
