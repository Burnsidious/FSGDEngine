#pragma once



namespace EDRendererD3D
{
	class BlendStateManager
	{
	public:
		enum BStates{BS_Default = 0, BS_Alpha, BS_Additive, BS_COUNT};

		~BlendStateManager(void);

		/// Gets a reference to the one and only IndexBuffer instance.
		static BlendStateManager &GetReference();
		static BlendStateManager *GetInstance();

		/// Deletes the instance of the IndexBuffer.
		static void DeleteInstance();

		bool ApplyState(BStates state);

		CComPtr<ID3D11BlendState > GetState(BStates state)
		{
			assert(state < BS_COUNT);
			return blendStates[state];
		}

		BStates GetCurrentState() { return currentState; }

	private:
		BlendStateManager(void);
		BlendStateManager(const BlendStateManager &) {}
		BlendStateManager &operator=(const BlendStateManager &) { return *this; }

		/// the one and only instance.
		static BlendStateManager *instancePtr;

		CComPtr<ID3D11BlendState > blendStates[BS_COUNT];
		BStates currentState;
		void CreateStates();
	};
}
